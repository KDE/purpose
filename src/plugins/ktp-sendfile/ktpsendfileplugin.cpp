/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <KLocalizedString>
#include <KPluginFactory>
#include <QJsonArray>
#include <QProcess>
#include <QStandardPaths>
#include <purpose/pluginbase.h>

class KTpSendFileShareJob : public Purpose::Job
{
    Q_OBJECT
public:
    KTpSendFileShareJob(QObject *parent)
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
        QString executable = QStandardPaths::findExecutable(QStringLiteral("ktp-send-file"));
        if (executable.isEmpty()) {
            setError(1);
            setErrorText(i18n("Couldn't find 'ktp-send-file' executable."));
            emitResult();
            return;
        }
        QProcess *process = new QProcess(this);
        process->setProgram(executable);
        QJsonArray urlsJson = data().value(QStringLiteral("urls")).toArray();
        process->setArguments(arrayToList(urlsJson));
        connect(process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &KTpSendFileShareJob::jobFinished);

        process->start();
    }

    Q_SLOT void jobFinished(int code, QProcess::ExitStatus /*status*/)
    {
        setError(code);
        setOutput({{QStringLiteral("url"), QString()}});
        emitResult();
    }
};

class Q_DECL_EXPORT KTpSendFilePlugin : public Purpose::PluginBase
{
    Q_OBJECT
public:
    KTpSendFilePlugin(QObject *p, const QVariantList &)
        : Purpose::PluginBase(p)
    {
    }

    Purpose::Job *createJob() const override
    {
        return new KTpSendFileShareJob(nullptr);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KTpSendFilePlugin, "ktpsendfileplugin.json")

#include "ktpsendfileplugin.moc"
