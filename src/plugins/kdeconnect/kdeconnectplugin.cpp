/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <KPluginFactory>
#include <QDebug>
#include <QJsonArray>
#include <QProcess>
#include <QStandardPaths>
#include <purpose/pluginbase.h>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

using namespace Qt::StringLiterals;

class KDEConnectJob : public Purpose::Job
{
    Q_OBJECT
public:
    KDEConnectJob(QObject *parent)
        : Purpose::Job(parent)
    {
    }

    QStringList arrayToList(const QJsonArray &array)
    {
        QStringList ret;
        for (const QJsonValue &val : array) {
            ret += val.toString();
        }
        return ret;
    }

    void start() override
    {
        const QString deviceId = data().value(QLatin1String("device")).toString();
        QJsonArray urlsJson = data().value(QLatin1String("urls")).toArray();

        QDBusMessage msg = QDBusMessage::createMethodCall(u"org.kde.kdeconnect"_s,
                                                          "/modules/kdeconnect/devices/"_L1 + deviceId + "/share"_L1,
                                                          u"org.kde.kdeconnect.device.share"_s,
                                                          u"shareUrls"_s);
        msg.setArguments({arrayToList(urlsJson)});

        QDBusPendingReply<> reply = QDBusConnection::sessionBus().asyncCall(msg);

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<> reply = *watcher;
            if (reply.isError()) {
                qWarning() << "kdeconnect share error:" << reply.error().message();
                setError(2);
                setErrorText(reply.error().message());
                emitResult();
            } else {
                setError(0);
                setOutput({{QStringLiteral("url"), QString()}});
                emitResult();
            }
        });
    }

private:
};

class KDEConnectPlugin : public Purpose::PluginBase
{
    Q_OBJECT
public:
    using PluginBase::PluginBase;
    Purpose::Job *createJob() const override
    {
        return new KDEConnectJob(nullptr);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KDEConnectPlugin, "kdeconnectplugin.json")

#include "kdeconnectplugin.moc"
