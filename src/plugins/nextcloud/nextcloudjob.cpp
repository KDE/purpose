/*
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "nextcloudjob.h"

#if HAVE_KACCOUNTS
#include <KAccounts/Core>
#include <KAccounts/GetCredentialsJob>
#endif

#include <KIO/CopyJob>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDebug>

using namespace Qt::Literals;

QList<QUrl> arrayToList(const QJsonArray &array)
{
    QList<QUrl> ret;
    for (const QJsonValue &val : array) {
        ret += val.toVariant().toUrl();
    }
    return ret;
}

bool usingKOnlineAccounts()
{
    return QDBusConnection::sessionBus().interface()->isServiceRegistered(u"org.kde.KOnlineAccounts"_s);
}

void NextcloudJob::start()
{
    if (!usingKOnlineAccounts()) {
#if HAVE_KACCOUNTS
        const QString idString = data().value(QLatin1String("accountId")).toString();
        const Accounts::AccountId id = idString.toInt();
        auto credentialsJob = new KAccounts::GetCredentialsJob(id, this);

        connect(credentialsJob, &KAccounts::GetCredentialsJob::finished, this, &NextcloudJob::gotCredentials);

        credentialsJob->start();
#endif
    } else {
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

        doUpload(destUrl);
    }
}

#if HAVE_KACCOUNTS
void NextcloudJob::gotCredentials(KJob *job)
{
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    const Accounts::AccountId id = data().value(QLatin1String("accountId")).toInt();
    Accounts::Account *acc = Accounts::Account::fromId(KAccounts::accountsManager(), id);

    const auto services = acc->services();
    for (const Accounts::Service &service : services) {
        if (service.name() == QStringLiteral("dav-storage")) {
            acc->selectService(service);
        }
    }

    KAccounts::GetCredentialsJob *credentialsJob = qobject_cast<KAccounts::GetCredentialsJob *>(job);
    Q_ASSERT(credentialsJob);
    const QString folder = data().value(QLatin1String("folder")).toString();

    QUrl destUrl;
    destUrl.setHost(acc->valueAsString(QStringLiteral("dav/host")));
    destUrl.setScheme(QStringLiteral("webdav"));
    destUrl.setPath(acc->valueAsString(QStringLiteral("dav/storagePath")) + folder);
    destUrl.setUserName(credentialsJob->credentialsData().value(QStringLiteral("UserName")).toString());
    destUrl.setPassword(credentialsJob->credentialsData().value(QStringLiteral("Secret")).toString());

    doUpload(destUrl);
}
#endif

void NextcloudJob::doUpload(const QUrl &destUrl)
{
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
