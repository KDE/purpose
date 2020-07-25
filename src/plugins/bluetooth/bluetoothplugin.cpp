/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <purpose/pluginbase.h>
#include <QDebug>
#include <QUrl>
#include <QProcess>
#include <QJsonArray>
#include <KPluginFactory>

EXPORT_SHARE_VERSION

class BluetoothJob : public Purpose::Job
{
    Q_OBJECT
    public:
        BluetoothJob(QObject* parent)
            : Purpose::Job(parent)
        {}

        QStringList arrayToList(const QJsonArray& array)
        {
            QStringList ret;
            for (const QJsonValue& val : array) {
                QUrl url(val.toString());
                if(url.isLocalFile()) {
                    ret += url.toLocalFile();
                }
            }
            return ret;
        }

        void start() override
        {
            QProcess* process = new QProcess(this);
            process->setProgram(QStringLiteral("bluedevil-sendfile"));
            QJsonArray urlsJson = data().value(QStringLiteral("urls")).toArray();
            process->setArguments(QStringList(QStringLiteral("-u")) << data().value(QStringLiteral("device")).toString() << QStringLiteral("-f") << arrayToList(urlsJson));
            connect(process, &QProcess::errorOccurred, this, &BluetoothJob::processError);
            connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &BluetoothJob::jobFinished);
            connect(process, &QProcess::readyRead, this, [process](){ qDebug() << "bluedevil-sendfile output:" << process->readAll(); });

            process->start();
        }

        void processError(QProcess::ProcessError error)
        {
            QProcess* process = qobject_cast<QProcess*>(sender());
            qWarning() << "bluetooth share error:" << error << process->errorString();
            setError(1 + error);
            setErrorText(process->errorString());
            emitResult();
        }

        void jobFinished(int code, QProcess::ExitStatus status)
        {
            if (status != QProcess::NormalExit)
                qWarning() << "bluedevil-sendfile crashed";

            setError(code);
            setOutput( {{ QStringLiteral("url"), QString() }});
            emitResult();
        }

    private:
};

class Q_DECL_EXPORT BluetoothPlugin : public Purpose::PluginBase
{
    Q_OBJECT
    public:
        BluetoothPlugin(QObject* p, const QVariantList& ) : Purpose::PluginBase(p) {}

        Purpose::Job* createJob() const override
        {
            return new BluetoothJob(nullptr);
        }
};

K_PLUGIN_CLASS_WITH_JSON(BluetoothPlugin, "bluetoothplugin.json")

#include "bluetoothplugin.moc"
