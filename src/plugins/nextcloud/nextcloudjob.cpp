/*
 Copyright 2017 Lim Yuen Hoe <yuenhoe86@gmail.com>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either 
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public 
 License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "nextcloudjob.h"
#include <QDebug>
#include <QFileInfo>
#include <KStringHandler>
#include <KLocalizedString>
#include <KAccounts/getcredentialsjob.h>
#include <KAccounts/core.h>
#include <KIO/Job>
#include <KIO/DavJob>

void NextcloudJob::start()
{
    // get all the info we need
    const QString folder = data().value(QStringLiteral("folder")).toString();
    const Accounts::AccountId id = data().value(QStringLiteral("accountId")).toInt();
    Accounts::Account* acc = Accounts::Account::fromId(KAccounts::accountsManager(), id);
    auto job = new GetCredentialsJob(id, this);
    bool b = job->exec();
    if (!b) {
        qWarning() << "Couldn't fetch credentials";
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }
    Q_FOREACH(const Accounts::Service &service, acc->services()) {
        if (service.name() == QStringLiteral("nextcloud-upload")) {
            acc->selectService(service);
        }
    }
    m_davUrl = QUrl(acc->valueAsString(QStringLiteral("server")) +
                    QStringLiteral("remote.php/webdav/") + folder + QStringLiteral("/"));
    m_davUrl.setUserName(job->credentialsData().value(QStringLiteral("UserName")).toString());
    m_davUrl.setPassword(job->credentialsData().value(QStringLiteral("Secret")).toString());
    
    // first we check that the folder exists
    KIO::DavJob* davjob = KIO::davPropFind(m_davUrl, QDomDocument(), QStringLiteral("0"), KIO::HideProgressInfo);
    connect(davjob, &KJob::finished, this, &NextcloudJob::checkTargetFolder);
}

void NextcloudJob::checkTargetFolder(KJob* j)
{
    QString responseString = qobject_cast<KIO::DavJob*>(j)->response().toString();
    // TODO: prob a better way to do this
    if (responseString.contains(QStringLiteral("<d:collection xmlns:d=\"DAV:\"/>"))) {
        const QJsonArray urls = data().value(QStringLiteral("urls")).toArray();

        foreach(const QJsonValue& url, urls) {
            // before uploading, we try to avoid overwrite by checking for existing file on nextcloud
            QUrl local = QUrl(url.toString());
            QUrl uploadTarget = m_davUrl;
            // disallow giant filenames
            // TODO: this doesn't currently deal well with say image clipboard data. might see if we can somehow add the right extension.
            uploadTarget.setPath(uploadTarget.path() + KStringHandler::csqueeze(local.fileName(), 100));
            KIO::DavJob* davjob = KIO::davPropFind(uploadTarget, QDomDocument(), QStringLiteral("0"), KIO::HideProgressInfo);
            connect(davjob, &KJob::finished, this, [=](KJob* job) { NextcloudJob::checkTargetFile(local, job); });
            m_pendingJobs++;
        }

    } else {
        qWarning() << "invalid folder";
        setError(KIO::Error::ERR_CANNOT_ENTER_DIRECTORY);
        setErrorText(i18n("Invalid folder!"));
        emitResult();
    }
}

void NextcloudJob::checkTargetFile(const QUrl& local, KJob* j)
{
    if (j->error()) {
        setError(j->error());
        setErrorText(j->errorText());
        emitResult();
        return;
    }

    KIO::DavJob* job = qobject_cast<KIO::DavJob*>(j);
    QString responseString = job->response().toString();

    // TODO: better way to do this
    if (responseString.contains(QStringLiteral("DAV\\Exception\\NotFound</s:exception>"))) {
        // okay file doesn't exist on nextcloud, we'll fetch the file, then upload it
        KIO::StoredTransferJob* next = KIO::storedGet(local);
        QUrl targetUrl = job->url();
        connect(next, &KJob::finished, this, [=](KJob* jj) { NextcloudJob::fileFetched(targetUrl, jj); });
    } else {
        // file already exists! we try successive suggestions until we find a free name
        QUrl uploadTarget = m_davUrl;
        uploadTarget.setPath(uploadTarget.path() + KIO::suggestName(m_davUrl, job->url().fileName()));
        qDebug() << "Trying: " << uploadTarget.toString();
        KIO::DavJob* davjob = KIO::davPropFind(uploadTarget, QDomDocument(), QStringLiteral("0"), KIO::HideProgressInfo);
        connect(davjob, &KJob::finished, this, [=](KJob* jj) { NextcloudJob::checkTargetFile(local, jj); });
    }
}

void NextcloudJob::fileFetched(const QUrl& uploadUrl, KJob* j)
{
    if (j->error()) {
        setError(j->error());
        setErrorText(j->errorText());
        emitResult();
        return;
    }

    KIO::StoredTransferJob* job = qobject_cast<KIO::StoredTransferJob*>(j);

    // we fetched our file and we have a place to upload to. Time to upload!
    KIO::StoredTransferJob *tJob = KIO::storedPut(job->data(), uploadUrl, KIO::HideProgressInfo);
    connect(tJob, &KJob::result, this, &NextcloudJob::fileUploaded);
}

void NextcloudJob::fileUploaded(KJob* j)
{
    if (j->error()) {
        setError(j->error());
        setErrorText(j->errorText());
        emitResult();
        return;
    }

    //KIO::StoredTransferJob *sjob = qobject_cast<KIO::StoredTransferJob *>(j);
    m_pendingJobs--;
    if (m_pendingJobs == 0) {
        setOutput( {{ QStringLiteral("url"), QString() }});
        emitResult();
    }
}
