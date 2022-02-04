/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <KLocalizedString>
#include <KPluginFactory>

#include <QDebug>
#include <QJsonArray>
#include <QProcess>
#include <QStandardPaths>
#include <purpose/pluginbase.h>

EXPORT_SHARE_VERSION

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
        const QString connectExec = QStandardPaths::findExecutable(QStringLiteral("kdeconnect-cli"));
        if (connectExec.isEmpty()) {
            setError(KJob::UserDefinedError);
            setErrorText(i18n("Couldn't find 'kdeconnect-cli' executable."));
            emitResult();
            return;
        }

        QProcess *process = new QProcess(this);
        process->setProgram(connectExec);
        QJsonArray urlsJson = data().value(QStringLiteral("urls")).toArray();
        process->setArguments(QStringList(QStringLiteral("--device"))
                              << data().value(QStringLiteral("device")).toString() << QStringLiteral("--share") << arrayToList(urlsJson));
        connect(process, &QProcess::errorOccurred, this, &KDEConnectJob::processError);
        connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, &KDEConnectJob::jobFinished);
        connect(process, &QProcess::readyRead, this, [process]() {
            qDebug() << "kdeconnect-cli output:" << process->readAll();
        });

        process->start();
    }

    void processError(QProcess::ProcessError error)
    {
        QProcess *process = qobject_cast<QProcess *>(sender());
        qWarning() << "kdeconnect share error:" << error << process->errorString();
        setError(1 + error);
        setErrorText(process->errorString());
        emitResult();
    }

    void jobFinished(int code, QProcess::ExitStatus status)
    {
        if (status != QProcess::NormalExit)
            qWarning() << "kdeconnect-cli crashed";

        setError(code);
        setOutput({{QStringLiteral("url"), QString()}});
        emitResult();
    }

private:
};

class Q_DECL_EXPORT KDEConnectPlugin : public Purpose::PluginBase
{
    Q_OBJECT
public:
    KDEConnectPlugin(QObject *p, const QVariantList &)
        : Purpose::PluginBase(p)
    {
    }

    Purpose::Job *createJob() const override
    {
        return new KDEConnectJob(nullptr);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KDEConnectPlugin, "kdeconnectplugin.json")

#include "kdeconnectplugin.moc"
