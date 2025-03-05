/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "youtubejobcomposite.h"
#include "youtubejob.h"

#include <KLocalizedString>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QStandardPaths>

using namespace Qt::StringLiterals;

YoutubeJobComposite::YoutubeJobComposite()
    : Purpose::Job()
{
}

void YoutubeJobComposite::start()
{
    const QJsonValue jsonId = data().value(QLatin1String("accountId"));
    if (jsonId.isNull() || jsonId.isUndefined()) {
        setError(1);
        setErrorText(i18n("No YouTube account configured in your accounts."));
        emitResult();
        return;
    }

    const QDBusObjectPath path(jsonId.toString());

    QDBusMessage msg = QDBusMessage::createMethodCall(u"org.kde.KOnlineAccounts"_s, path.path(), u"org.freedesktop.DBus.Properties"_s, u"GetAll"_s);
    msg.setArguments({u"org.kde.KOnlineAccounts.Google"_s});
    QDBusReply<QVariantMap> reply = QDBusConnection::sessionBus().call(msg);

    const QVariantMap result = reply.value();

    qWarning() << "re" << reply.error() << result;

    const QByteArray accessToken = result[u"accessToken"_s].toString().toUtf8();

    qWarning() << "got token" << accessToken;

    m_pendingJobs = 0;
    const QJsonArray urls = data().value(QLatin1String("urls")).toArray();
    for (const QJsonValue &url : urls) {
        YoutubeJob *job = new YoutubeJob(QUrl(url.toString()),
                                         accessToken,
                                         data().value(QLatin1String("videoTitle")).toString(),
                                         data().value(QLatin1String("videoTags")).toString().split(QLatin1Char(',')),
                                         data().value(QLatin1String("videoDesc")).toString(),
                                         this);
        connect(job, &KJob::finished, this, &YoutubeJobComposite::subjobFinished);
        job->start();
        m_pendingJobs++;
    }
}

void YoutubeJobComposite::subjobFinished(KJob *subjob)
{
    m_pendingJobs--;
    if (subjob->error()) {
        setError(subjob->error());
        setErrorText(subjob->errorText());
        emitResult();
        return;
    }
    if (m_pendingJobs == 0) {
        if (!error()) {
            const QJsonValue url = qobject_cast<YoutubeJob *>(subjob)->outputUrl();
            setOutput({{QStringLiteral("url"), url.toString()}});
        }
        emitResult();
    }
}
