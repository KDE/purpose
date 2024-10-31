/*
    SPDX-FileCopyrightText: 2024 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <KLocalizedString>
#include <KPluginFactory>

#include <QGuiApplication>
#include <QJsonArray>
#include <QStandardPaths>
#include <QTimer>
#include <purpose/pluginbase.h>
#include <qclipboard.h>

class ClipboardJob : public Purpose::Job
{
    Q_OBJECT
public:
    ClipboardJob(QObject *parent)
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
        const QJsonArray urlsJson = data().value(QLatin1String("urls")).toArray();
        const auto payload = arrayToList(urlsJson).join(u' ');
        QTimer::singleShot(0, this, [this, payload] {
            qGuiApp->clipboard()->setText(payload);
            emitResult();
        });
    }
};

class ClipboardPlugin : public Purpose::PluginBase
{
    Q_OBJECT
public:
    using PluginBase::PluginBase;
    Purpose::Job *createJob() const override
    {
        return new ClipboardJob(nullptr);
    }
};

K_PLUGIN_FACTORY_WITH_JSON(Clipboard, "clipboardplugin.json", registerPlugin<ClipboardPlugin>();)

#include "clipboardplugin.moc"
