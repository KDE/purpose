/*
    SPDX-FileCopyrightText: 2024 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <KLocalizedString>
#include <KPluginFactory>

#include <QClipboard>
#include <QFile>
#include <QGuiApplication>
#include <QImage>
#include <QJsonArray>
#include <QTimer>
#include <purpose/pluginbase.h>

class ClipboardJob : public Purpose::Job
{
    Q_OBJECT
public:
    ClipboardJob(QObject *parent)
        : Purpose::Job(parent)
    {
    }

    QList<QUrl> arrayToList(const QJsonArray &array)
    {
        QList<QUrl> ret;
        ret.reserve(array.size());
        for (const QJsonValue &val : array) {
            ret += QUrl(val.toString());
        }
        return ret;
    }

    void start() override
    {
        QTimer::singleShot(0, this, [this] {
            QMimeData *mimeData = new QMimeData;
            mimeData->setUrls(arrayToList(data().value(QLatin1String("urls")).toArray()));
            qGuiApp->clipboard()->setMimeData(mimeData);
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
