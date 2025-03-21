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

    void start() override
    {
        QTimer::singleShot(0, this, [this] {
            QMimeData *mimeData = new QMimeData;

            QString text;
            QList<QUrl> urls;
            for (const QJsonValue &jsonUrl : data().value(QLatin1String("urls")).toArray()) {
                const QString jsonUrlString = jsonUrl.toString();

                const QUrl url{jsonUrlString};
                // Keep TolerantMode for QUrl but filter out obvious garbage.
                if (!url.scheme().isEmpty()) {
                    urls.append(url);
                }
                if (!text.isEmpty()) {
                    text.append(QLatin1Char('\n'));
                }
                text.append(jsonUrlString);
            }

            mimeData->setText(text);
            mimeData->setUrls(urls);
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
