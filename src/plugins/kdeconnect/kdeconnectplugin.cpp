/*
 Copyright 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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

EXPORT_SHARE_VERSION

class KDEConnectJob : public Purpose::Job
{
    Q_OBJECT
    public:
        KDEConnectJob(QObject* parent)
            : Purpose::Job(parent)
        {}

        QStringList arrayToList(const QJsonArray& array)
        {
            QStringList ret;
            foreach(const QJsonValue& val, array) {
                ret += val.toString();
            }
            return ret;
        }

        void start() override
        {
            QProcess* process = new QProcess(this);
            process->setProgram(QStringLiteral("kdeconnect-cli"));
            QJsonArray urlsJson = data().value(QStringLiteral("urls")).toArray();
            process->setArguments(QStringList(QStringLiteral("--device")) << data().value(QStringLiteral("device")).toString() << QStringLiteral("--share") << arrayToList(urlsJson));
            connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &KDEConnectJob::jobFinished);
            connect(process, &QProcess::readyRead, this, [process](){ qDebug() << "kdeconnect-cli output:" << process->readAll(); });

            process->start();
        }

        void jobFinished(int code, QProcess::ExitStatus status)
        {
            if (status != QProcess::NormalExit)
                qWarning() << "kdeconnect-cli crashed";

            setError(code);
            setOutput( {{ QStringLiteral("url"), QString() }});
            emitResult();
        }

    private:
};

class Q_DECL_EXPORT KDEConnectPlugin : public Purpose::PluginBase
{
    Q_OBJECT
    public:
        KDEConnectPlugin(QObject* p, const QVariantList& ) : Purpose::PluginBase(p) {}

        Purpose::Job* createJob() const override
        {
            return new KDEConnectJob(nullptr);
        }
};

K_PLUGIN_FACTORY_WITH_JSON(KDEConnect, "kdeconnectplugin.json", registerPlugin<KDEConnectPlugin>();)

#include "kdeconnectplugin.moc"
