/*************************************************************************************
 *  Copyright (C) 2008-2011 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2008-2011 by Alex Fiestas <afiestas@kde.org>                       *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include "youtubejobcomposite.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QStandardPaths>
#include <KLocalizedString>
#include <KAccounts/getcredentialsjob.h>
#include <KAccounts/core.h>
#include <Accounts/Manager>
#include <Accounts/Application>

QDebug operator<<(QDebug s, const Accounts::Service& service)
{
    s.nospace() << qPrintable(service.displayName()) << ',' << qPrintable(service.name()) << '\n';
    return s;
}
QDebug operator<<(QDebug s, const Accounts::Provider& provider)
{
    s.nospace() << "Provider(" << qPrintable(provider.displayName()) << ',' << qPrintable(provider.name()) << ")\n";
    return s;
}

YoutubeJobComposite::YoutubeJobComposite()
    : Purpose::Job()
{
}

void YoutubeJobComposite::start()
{
    //TODO Make it possible to configure the accountid
    Accounts::AccountId id;
    {
        Accounts::Manager* mgr = KAccounts::accountsManager();
        auto accounts =  mgr->accountList(QStringLiteral("google-youtube"));
        if (accounts.isEmpty()) {
            setError(1);
            setErrorText(i18n("No YouTube account configured in your accounts."));
            emitResult();
            return;
        }
        id = accounts.first();
    }

    //TODO: make async
    QByteArray accessToken;
    {
        auto job = new GetCredentialsJob(id, this);
        bool b = job->exec();
        if (!b) {
            qWarning() << "Couldn't fetch credentials";
            setError(job->error());
            setErrorText(job->errorText());
            emitResult();
            return;
        }
        accessToken = job->credentialsData()[QStringLiteral("AccessToken")].toByteArray();
    }

    m_pendingJobs = 0;
    const QJsonArray urls = data().value(QStringLiteral("urls")).toArray();
    foreach(const QJsonValue& url, urls) {
        YoutubeJob* job = new YoutubeJob(QUrl(url.toString()),
                                         accessToken,
                                         data().value(QStringLiteral("videoTitle")).toString(),
                                         data().value(QStringLiteral("videoTags")).toString().split(QLatin1Char(',')),
                                         data().value(QStringLiteral("videoDesc")).toString(), this);
        connect(job, &KJob::finished, this, &YoutubeJobComposite::subjobFinished);
        job->start();
        m_pendingJobs++;
    }
}

void YoutubeJobComposite::subjobFinished(KJob* subjob)
{
    m_pendingJobs--;
    if (subjob->error()) {
        setError(subjob->error());
        setErrorText(subjob->errorText());
        emitResult();
        return;
    }
    if (m_pendingJobs==0) {
        if (!error()) {
            const QJsonValue url = qobject_cast<YoutubeJob*>(subjob)->outputUrl();
            Q_EMIT output({{ QStringLiteral("url"), url.toString() }});
        }
        emitResult();
    }
}

QUrl YoutubeJobComposite::configSourceCode() const
{
    QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("purpose/youtubeplugin_config.qml"));
    Q_ASSERT(!path.isEmpty());
    return QUrl::fromLocalFile(path);
}
