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
#include <KLocalizedString>
#include <KPluginFactory>

EXPORT_SHARE_VERSION

class KTpSendFileShareJob : public Purpose::Job
{
    Q_OBJECT
    public:
        KTpSendFileShareJob(QObject* parent)
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
            QString executable = QStandardPaths::findExecutable(QStringLiteral("ktp-send-file"));
            if (executable.isEmpty()) {
                setError(1);
                setErrorText(i18n("Couldn't find 'ktp-send-file' executable."));
                emitResult();
                return;
            }
            QProcess* process = new QProcess(this);
            process->setProgram(executable);
            QJsonArray urlsJson = data().value(QStringLiteral("urls")).toArray();
            process->setArguments(arrayToList(urlsJson));
            connect(process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &KTpSendFileShareJob::jobFinished);

            process->start();
        }

        Q_SLOT void jobFinished(int code, QProcess::ExitStatus /*status*/)
        {
            setError(code);
            setOutput( {{ QStringLiteral("url"), QString() }});
            emitResult();
        }
};

class Q_DECL_EXPORT KTpSendFilePlugin : public Purpose::PluginBase
{
    Q_OBJECT
    public:
        KTpSendFilePlugin(QObject* p, const QVariantList& ) : Purpose::PluginBase(p) {}

        Purpose::Job* createJob() const override
        {
            return new KTpSendFileShareJob(nullptr);
        }
};

K_PLUGIN_CLASS_WITH_JSON(KTpSendFilePlugin, "ktpsendfileplugin.json")

#include "ktpsendfileplugin.moc"
