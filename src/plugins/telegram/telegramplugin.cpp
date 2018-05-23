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
            foreach(const QJsonValue& val, array) {
                QUrl url(val.toString());
                if(url.isLocalFile()) {
                    ret += url.toLocalFile();
                }
            }
            return ret;
        }

        void start() override
        {
            // Try executing via telegram-desktop command. If it fails, try again with flatpak commands.
            // TODO Add snap command
            tryClassic();
        }

        void tryClassic()
        {
            QJsonArray urlsJson = data().value(QStringLiteral("urls")).toArray();
            QString classicCommand(QStringLiteral("telegram-desktop"));
            QStringList classicArgs(QStringLiteral("-sendpath"));
            classicArgs << arrayToList(urlsJson);

            QProcess* process = new QProcess(this);
            process->setProgram(classicCommand);
            process->setArguments(classicArgs);
            connect(process, &QProcess::errorOccurred, this, &TelegramJob::tryFlatpak);
            connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &TelegramJob::jobFinishedClassic);
            connect(process, &QProcess::readyRead, this, [process](){ qDebug() << "telegram-desktop output:" << process->readAll(); });

            process->start();
        }

        void tryFlatpak()
        {
            QJsonArray urlsJson = data().value(QStringLiteral("urls")).toArray();
            QString classicCommand(QStringLiteral("/usr/bin/flatpak"));
            QStringList classicArgs(QStringLiteral("run"));
            classicArgs << QStringLiteral("--file-forwarding") << QStringLiteral("org.telegram.desktop") << QStringLiteral("-sendpath")
                        << QStringLiteral("@@") << arrayToList(urlsJson) << QStringLiteral("@@");

            QProcess* process = new QProcess(this);
            process->setProgram(classicCommand);
            process->setArguments(classicArgs);
            connect(process, &QProcess::errorOccurred, this, &TelegramJob::processError);
            connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &TelegramJob::jobFinished);
            connect(process, &QProcess::readyRead, this, [process](){ qDebug() << "telegram-desktop output:" << process->readAll(); });

            process->start();
        }

        void jobFinishedClassic(int code, QProcess::ExitStatus status)
        {
            if (status != QProcess::NormalExit) {
                return;
            }
            jobFinished(code, status);
        }

        void processError(QProcess::ProcessError error)
        {
            QProcess* process = qobject_cast<QProcess*>(sender());
            qWarning() << "telegram share error:" << error << process->errorString();
            setError(1 + error);
            setErrorText(process->errorString());
            emitResult();
        }

        void jobFinished(int code, QProcess::ExitStatus status)
        {
            if (status != QProcess::NormalExit)
                qWarning() << "Telegram not found or crashing";

            setError(code);
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

K_PLUGIN_FACTORY_WITH_JSON(Telegram, "telegramplugin.json", registerPlugin<TelegramPlugin>();)

#include "telegramplugin.moc"
