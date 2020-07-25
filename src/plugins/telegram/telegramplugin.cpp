/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <purpose/pluginbase.h>
#include <QDebug>
#include <QProcess>
#include <QJsonArray>
#include <QStandardPaths>
#include <KPluginFactory>
#include <KDesktopFile>
#include <KConfigGroup>
#include <KShell>
#include <KLocalizedString>
#include <QTimer>
#include <QUrl>
#include "debug.h"

Q_LOGGING_CATEGORY(PLUGIN_TELEGRAM, "kf.purpose.plugins.telegram")

EXPORT_SHARE_VERSION

class TelegramJob : public Purpose::Job
{
    Q_OBJECT
    public:
        TelegramJob(QObject* parent)
            : Purpose::Job(parent)
        {}

        QStringList arrayToList(const QJsonArray& array)
        {
            QStringList ret;
            for (const QJsonValue& val : array) {
                QUrl url(val.toString());
                if(url.isLocalFile()) {
                    ret += KShell::quoteArg(url.toLocalFile());
                }
            }
            return ret;
        }

        void start() override
        {
            for (const QString &desktopFile: {QStringLiteral("org.telegram.desktop.desktop"), QStringLiteral("telegramdesktop.desktop"), QStringLiteral("telegram-desktop.desktop") }) {
                const auto path = QStandardPaths::locate(QStandardPaths::ApplicationsLocation, desktopFile);
                if (!path.isEmpty()) {
                    useDesktopFile(path);
                    return;
                }
            }

            //Failed to find the application
            QTimer::singleShot(0, this, [this]() {
                setError(1);
                setErrorText(i18n("Could not find telegram"));
                setOutput( {{ QStringLiteral("url"), QString() }});
                emitResult();
            });
        }

        void useDesktopFile(const QString &path)
        {
            const KDesktopFile file(path);
            const KConfigGroup desktopEntryGroup = file.group("Desktop Entry");
            QString execLine = desktopEntryGroup.readEntry("Exec");
            execLine.replace(QLatin1String("%u"), arrayToList(data().value(QStringLiteral("urls")).toArray()).join(QLatin1Char(' ')));
            execLine.replace(QLatin1String("@@u"), QLatin1String("@@"));
            execLine.replace(QLatin1String(" -- "), QLatin1String(" -sendpath "));

            QStringList args = KShell::splitArgs(execLine);

            QProcess* process = new QProcess(this);
            process->setProgram(args.takeFirst());
            process->setArguments(args);
            connect(process, &QProcess::errorOccurred, this, &TelegramJob::processError);
            connect(process, &QProcess::readyRead, this, [process](){ qDebug() << process->program() << "output:" << process->readAll(); });

            process->start();
            QTimer::singleShot(500, this, &TelegramJob::jobFinished);
        }

        void processError(QProcess::ProcessError error)
        {
            QProcess* process = qobject_cast<QProcess*>(sender());
            qWarning() << "telegram share error:" << error << process->errorString();
            setError(1 + error);
            setErrorText(process->errorString());
            emitResult();
        }

        void jobFinished()
        {
            setOutput( {{ QStringLiteral("url"), QString() }});
            emitResult();
        }

    private:
};

class Q_DECL_EXPORT TelegramPlugin : public Purpose::PluginBase
{
    Q_OBJECT
    public:
        TelegramPlugin(QObject* p, const QVariantList& ) : Purpose::PluginBase(p) {}

        Purpose::Job* createJob() const override
        {
            return new TelegramJob(nullptr);
        }
};

K_PLUGIN_CLASS_WITH_JSON(TelegramPlugin, "telegramplugin.json")

#include "telegramplugin.moc"
