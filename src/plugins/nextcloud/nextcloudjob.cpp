/*
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "nextcloudjob.h"
#include <KIO/CopyJob>
#include <QDebug>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusReply>

using namespace Qt::Literals;

QList<QUrl> arrayToList(const QJsonArray &array)
{
    QList<QUrl> ret;
    for (const QJsonValue &val : array) {
        ret += val.toVariant().toUrl();
    }
    return ret;
}

void NextcloudJob::start()
{
    const QDBusObjectPath path(data().value(QLatin1String("accountId")).toString());

    QDBusMessage msg = QDBusMessage::createMethodCall(u"org.kde.KOnlineAccounts"_s, path.path(), u"org.freedesktop.DBus.Properties"_s, u"GetAll"_s);
    msg.setArguments({u"org.kde.KOnlineAccounts.Nextcloud"_s});
    QDBusReply<QVariantMap> reply = QDBusConnection::sessionBus().call(msg);

    const QVariantMap result = reply.value();
    const QString folder = data().value(QLatin1String("folder")).toString();
    const QString storagePath = result[u"storagePath"_s].toString();
    const QString username = result[u"username"_s].toString();
    const QString password = result[u"password"_s].toString();

    QUrl destUrl = result[u"url"_s].toUrl();
    destUrl.setScheme(QStringLiteral("webdavs"));
    destUrl.setPath(storagePath + u"/" + folder); // TODO this discards subpaths
    destUrl.setUserName(username);
    destUrl.setPassword(password);

    const QList<QUrl> sourceUrls = arrayToList(data().value(QLatin1String("urls")).toArray());

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

#include "moc_nextcloudjob.cpp"
