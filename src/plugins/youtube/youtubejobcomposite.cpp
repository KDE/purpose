/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "youtubejobcomposite.h"
#include "youtube_debug.h"
#include "youtubejob.h"

#if HAVE_KACCOUNTS
#include <Accounts/Application>
#include <Accounts/Manager>
#include <KAccounts/Core>
#include <KAccounts/GetCredentialsJob>
#endif

#include <KLocalizedString>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusUnixFileDescriptor>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonValue>
#include <QStandardPaths>

using namespace Qt::StringLiterals;

#if HAVE_KACCOUNTS
QDebug operator<<(QDebug s, const Accounts::Service &service)
{
    s.nospace() << qPrintable(service.displayName()) << ',' << qPrintable(service.name()) << '\n';
    return s;
}
QDebug operator<<(QDebug s, const Accounts::Provider &provider)
{
    s.nospace() << "Provider(" << qPrintable(provider.displayName()) << ',' << qPrintable(provider.name()) << ")\n";
    return s;
}
#endif

bool usingKOnlineAccounts()
{
    return QDBusConnection::sessionBus().interface()->isServiceRegistered(u"org.kde.KOnlineAccounts"_s);
}

YoutubeJobComposite::YoutubeJobComposite()
    : Purpose::Job()
{
}

void YoutubeJobComposite::start()
{
    const QString idString = data().value(QLatin1String("accountId")).toString();
    if (idString.isEmpty()) {
        setError(1);
        setErrorText(i18n("No YouTube account configured in your accounts."));
        emitResult();
        return;
    }
    if (!usingKOnlineAccounts()) {
#if HAVE_KACCOUNTS
        const Accounts::AccountId id = idString.toInt();
        // TODO: make async
        QByteArray accessToken;
        {
            auto job = new KAccounts::GetCredentialsJob(id, this);
            bool b = job->exec();
            if (!b) {
                qCWarning(PLUGIN_YOUTUBE) << "Couldn't fetch credentials";
                setError(job->error());
                setErrorText(job->errorText());
                emitResult();
                return;
            }
            accessToken = job->credentialsData().value(QStringLiteral("AccessToken")).toByteArray();
        }

        doUpload(accessToken);
#endif
    } else {
        const QDBusObjectPath path(idString);

        QDBusMessage passwordMsg =
            QDBusMessage::createMethodCall(u"org.kde.KOnlineAccounts"_s, path.path(), u"org.kde.KOnlineAccounts.Google"_s, u"accessToken"_s);

        QDBusReply<QDBusUnixFileDescriptor> passwordReply = QDBusConnection::sessionBus().call(passwordMsg);

        if (!passwordReply.isValid()) {
            qCWarning(PLUGIN_YOUTUBE()) << "Failed to access token for account" << passwordReply.error().message();
            return;
        }

        QFile file;
        const bool openResult = file.open(passwordReply.value().fileDescriptor(), QFile::ReadOnly, QFile::AutoCloseHandle);

        if (!openResult) {
            qCWarning(PLUGIN_YOUTUBE) << "Could not open password fd" << file.errorString();
            return;
        }

        const QByteArray accessToken = file.readAll();

        Q_ASSERT(!accessToken.isEmpty());

        doUpload(accessToken);
    }
}

void YoutubeJobComposite::doUpload(const QByteArray &accessToken)
{
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
