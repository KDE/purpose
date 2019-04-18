/*
 Copyright 2018 Nicolas Fella <nicolas.fella@gmx.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <purpose/pluginbase.h>
#include <QDebug>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <KPluginFactory>
#include <KDesktopFile>
#include <KConfigGroup>
#include <KShell>
#include <KLocalizedString>
#include <QTimer>
#include "debug.h"

Q_LOGGING_CATEGORY(PLUGIN_TELEGRAM, "org.kde.purpose.plugin.telegram")

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
            for (const QString &desktopFile: {QStringLiteral("org.telegram.desktop.desktop"), QStringLiteral("telegramdesktop.desktop") }) {
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
