/*
    SPDX-FileCopyrightText: 2017 René J.V. Bertin <rjvbertin@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QFileInfo>
#include <QJsonArray>
#include <QStandardPaths>
#include <QUrl>

#include <KLocalizedString>
#include <KPluginFactory>

#include "debug.h"
#include "phabricatorjobs.h"

#include "purpose/job.h"
#include "purpose/pluginbase.h"

// FIXME: For some reason PLUGIN_PHABRICATOR isn't exported from the PhabricatorHelpers lib
#undef qCDebug
#define qCDebug(dum) qDebug()
#undef qCWarning
#define qCWarning(dum) qWarning()
#undef qCCritical
#define qCCritical(dum) qCritical()

class PhabricatorJob : public Purpose::Job
{
    Q_OBJECT
public:
    PhabricatorJob(QObject *object = nullptr)
        : Purpose::Job(object)
    {
    }

    void start() override
    {
        const QString localBaseDir(data().value(QStringLiteral("localBaseDir")).toString());
        const QUrl sourceFile(data().value(QStringLiteral("urls")).toArray().first().toString());
        const QString updateDR = data().value(QStringLiteral("updateDR")).toString();
        const bool doBrowse = data().value(QStringLiteral("doBrowse")).toBool();

        const QString baseDir = QUrl(localBaseDir).toLocalFile();

        if (QFileInfo(sourceFile.toLocalFile()).size() <= 0) {
            setError(KJob::UserDefinedError + 1);
            setErrorText(i18n("Phabricator refuses empty patchfiles"));
            Q_EMIT PhabricatorJob::warning(this, errorString(), QString());
            qCCritical(PLUGIN_PHABRICATOR) << errorString();
            emitResult();
            return;
        } else if (updateDR.localeAwareCompare(i18n("unknown")) == 0) {
            setError(KJob::UserDefinedError + 1);
            setErrorText(i18n("Please choose between creating a new revision or updating an existing one"));
            Q_EMIT PhabricatorJob::warning(this, errorString(), QString());
            qCCritical(PLUGIN_PHABRICATOR) << errorString();
            emitResult();
            return;
        }

        m_drTitle = data().value(QStringLiteral("drTitle")).toString();

        KJob *job;
        if (!updateDR.isEmpty()) {
            const QString updateComment = data().value(QStringLiteral("updateComment")).toString();
            job = new Phabricator::UpdateDiffRev(sourceFile, baseDir, updateDR, updateComment, doBrowse, this);
            connect(job, &KJob::finished, this, &PhabricatorJob::diffUpdated);
        } else {
            job = new Phabricator::NewDiffRev(sourceFile, baseDir, true, this);
            connect(job, &KJob::finished, this, &PhabricatorJob::diffCreated);
        }
        job->start();
        Q_EMIT PhabricatorJob::infoMessage(this, QStringLiteral("upload job started"), QString());
    }

    void diffCreatedOrUpdated(KJob *j, bool created)
    {
        if (j->error() != 0) {
            setError(j->error());
            setErrorText(j->errorString());
            Q_EMIT PhabricatorJob::warning(this, j->errorString(), QString());
            qCCritical(PLUGIN_PHABRICATOR) << "Could not upload the patch" << j->errorString();
            emitResult();
            return;
        }

        if (created) {
            Phabricator::NewDiffRev const *job = qobject_cast<Phabricator::NewDiffRev *>(j);
            qCWarning(PLUGIN_PHABRICATOR) << "new diff:" << job->diffURI();
            setOutput({{QStringLiteral("url"), job->diffURI()}});
        } else {
            Phabricator::UpdateDiffRev const *job = qobject_cast<Phabricator::UpdateDiffRev *>(j);
            qCWarning(PLUGIN_PHABRICATOR) << "updated diff" << job->requestId() << ":" << job->diffURI();
            setOutput({{QStringLiteral("url"), job->diffURI()}});
            Q_EMIT PhabricatorJob::infoMessage(this, QStringLiteral("updated diff %1: %2").arg(job->requestId()).arg(job->diffURI()), QString());
        }
        emitResult();
    }

    void diffCreated(KJob *j)
    {
        diffCreatedOrUpdated(j, true);
    }

    void diffUpdated(KJob *j)
    {
        diffCreatedOrUpdated(j, false);
    }

    QString m_drTitle;
};

class Q_DECL_EXPORT PhabricatorPlugin : public Purpose::PluginBase
{
    Q_OBJECT
public:
    PhabricatorPlugin(QObject *parent, const QList<QVariant> & /*args*/)
        : PluginBase(parent)
    {
    }
    ~PhabricatorPlugin() override
    {
    }

    Purpose::Job *createJob() const override
    {
        return new PhabricatorJob;
    }
};

K_PLUGIN_CLASS_WITH_JSON(PhabricatorPlugin, "phabricatorplugin.json")

#include "phabricatorplugin.moc"
