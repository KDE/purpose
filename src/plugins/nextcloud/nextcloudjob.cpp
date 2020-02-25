/*
 Copyright 2020 Nicolas Fella <nicolas.fella@gmx.de>

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
#include <KAccounts/GetCredentialsJob>
#include <KAccounts/Core>
#include <KIO/CopyJob>

QList<QUrl> arrayToList(const QJsonArray& array)
{
    QList<QUrl> ret;
    for (const QJsonValue& val : array) {
        ret += val.toVariant().toUrl();
    }
    return ret;
}

void NextcloudJob::start()
{
    const Accounts::AccountId id = data().value(QStringLiteral("accountId")).toInt();
    auto credentialsJob = new GetCredentialsJob(id, this);

    connect(credentialsJob, &GetCredentialsJob::finished, this, &NextcloudJob::gotCredentials);

    credentialsJob->start();
}

void NextcloudJob::gotCredentials(KJob *job)
{
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    const Accounts::AccountId id = data().value(QStringLiteral("accountId")).toInt();
    Accounts::Account* acc = Accounts::Account::fromId(KAccounts::accountsManager(), id);

    const auto services = acc->services();
    for (const Accounts::Service &service : services) {
        if (service.name() == QStringLiteral("dav-storage")) {
            acc->selectService(service);
        }
    }

    GetCredentialsJob *credentialsJob = qobject_cast<GetCredentialsJob *>(job);
    Q_ASSERT(credentialsJob);
    const QString folder = data().value(QStringLiteral("folder")).toString();

    QUrl destUrl;
    destUrl.setHost(acc->valueAsString(QStringLiteral("dav/host")));
    destUrl.setScheme(QStringLiteral("webdav"));
    destUrl.setPath(acc->valueAsString(QStringLiteral("dav/storagePath")) + folder);
    destUrl.setUserName(credentialsJob->credentialsData().value(QStringLiteral("UserName")).toString());
    destUrl.setPassword(credentialsJob->credentialsData().value(QStringLiteral("Secret")).toString());

    const QList<QUrl> sourceUrls = arrayToList(data().value(QStringLiteral("urls")).toArray());

    KIO::CopyJob *copyJob = KIO::copy(sourceUrls, destUrl);

    connect(copyJob, &KIO::CopyJob::finished, this, [this, copyJob] {
        if (copyJob->error()) {
            setError(copyJob->error());
            setErrorText(copyJob->errorText());
        }
        emitResult();
    });

    copyJob->start();
}
