/*
 * This file is part of KDevelop
 * Copyright 2017 René J.V. Bertin <rjvbertin@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <QDialog>
#include <QUrl>
#include <QVariantList>
#include <QStandardPaths>
#include <QJsonArray>

#include <KLocalizedString>
#include <KPluginFactory>
#include <QJsonDocument>

#include "phabricatorjobs.h"
#include "debug.h"

#include "purpose/job.h"
#include "purpose/pluginbase.h"

class PhabricatorJob : public Purpose::Job
{
    Q_OBJECT
    public:
        PhabricatorJob(QObject* object = Q_NULLPTR)
            : Purpose::Job(object)
    {}

    void start() override
    {
        const QString baseDir(data().value(QStringLiteral("baseDir")).toString());
        const QUrl sourceFile(data().value(QStringLiteral("urls")).toArray().first().toString());
        const QString updateDR = data().value(QStringLiteral("updateDR")).toString();

        KJob* job;
        if (!updateDR.isEmpty()) {
            // TODO: add update comment
            job=new Phabricator::UpdateDiffRev(sourceFile, baseDir, updateDR);
            connect(job, &KJob::finished, this, &PhabricatorJob::diffUpdated);
        } else {
            job=new Phabricator::NewDiffRev(sourceFile, baseDir);
            connect(job, &KJob::finished, this, &PhabricatorJob::diffCreated);
        }
        job->start();
    }

    void diffCreatedOrUpdated(KJob* j, bool created)
    {
        if(j->error()!=0) {
            setError(j->error());
            setErrorText(j->errorString());
            emitResult();
            return;
        }

        if (created) {
            Phabricator::NewDiffRev const * job = qobject_cast<Phabricator::NewDiffRev*>(j);
            setOutput({{ QStringLiteral("url"), job->diffURI() }});
        } else {
            Phabricator::UpdateDiffRev const * job = qobject_cast<Phabricator::UpdateDiffRev*>(j);
            setOutput({{ QStringLiteral("url"), job->diffURI() }});
        }
        emitResult();
    }

    void diffCreated(KJob* j)
    {
        diffCreatedOrUpdated(j, true);
    }

    void diffUpdated(KJob* j)
    {
        diffCreatedOrUpdated(j, false);
    }
};

class Q_DECL_EXPORT PhabricatorPlugin : public Purpose::PluginBase
{
    Q_OBJECT
    public:
        PhabricatorPlugin(QObject* parent, const QList<QVariant>& /*args*/) : PluginBase(parent) {}
        virtual ~PhabricatorPlugin() override {}

        virtual Purpose::Job* createJob() const override
        {
            return new PhabricatorJob;
        }
};

K_PLUGIN_FACTORY_WITH_JSON(PhabricatorPluginFactory, "phabricatorplugin.json", registerPlugin<PhabricatorPlugin>();)

#include "phabricatorplugin.moc"
