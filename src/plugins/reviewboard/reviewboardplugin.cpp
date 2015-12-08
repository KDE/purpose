/*
 * This file is part of KDevelop
 * Copyright 2010 Aleix Pol Gonzalez <aleixpol@kde.org>
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

#include "reviewboardjobs.h"
#include "debug.h"

#include "purpose/job.h"
#include "purpose/pluginbase.h"

class TheReviewboardJob : public Purpose::Job
{
    Q_OBJECT
    public:
        TheReviewboardJob(QObject* object = Q_NULLPTR)
            : Purpose::Job(object)
    {}

    void start() override
    {
        const QString baseDir(data().value(QStringLiteral("baseDir")).toString());
        const QString repository(data().value(QStringLiteral("repository")).toString());
        const QUrl sourceFile(data().value(QStringLiteral("urls")).toArray().first().toString());
        const QString updateRR = data().value(QStringLiteral("updateRR")).toString();
        const QJsonObject extraData = data().value(QStringLiteral("extraData")).toObject();
        const QString username = data().value(QStringLiteral("username")).toString();
        const QString password = data().value(QStringLiteral("password")).toString();
        QUrl server(data().value(QStringLiteral("server")).toString());
        server.setUserInfo(username + QLatin1Char(':') + password);

        KJob* job;
        if (!updateRR.isEmpty()) {
            job=new ReviewBoard::SubmitPatchRequest(server, sourceFile, baseDir, updateRR);
            connect(job, &KJob::finished, this, &TheReviewboardJob::reviewDone);
        } else {
            job=new ReviewBoard::NewRequest(server, repository);
            job->setProperty("extraData", extraData);
            connect(job, &KJob::finished, this, &TheReviewboardJob::reviewCreated);
        }
        job->setProperty("baseDir", baseDir);
        job->start();
    }

    void reviewCreated(KJob* j)
    {
        if (j->error()!=0) {
            setError(j->error());
            setErrorText(j->errorString());
            emitResult();
            return;
        }

        ReviewBoard::NewRequest const * job = qobject_cast<ReviewBoard::NewRequest*>(j);

        //This will provide things like groups and users for review from .reviewboardrc
        QVariantMap extraData = job->property("extraData").toMap();
        if (!extraData.isEmpty()) {
            KJob* updateJob = new ReviewBoard::UpdateRequest(job->server(), job->requestId(), extraData);
            updateJob->start();
        }

        // for git projects, m_source will be a VCSDiffPatchSource instance
        const QUrl sourceFile(data().value(QStringLiteral("urls")).toArray().first().toString());
        ReviewBoard::SubmitPatchRequest* submitPatchJob=new ReviewBoard::SubmitPatchRequest(job->server(), sourceFile, j->property("baseDir").toString(), job->requestId());
        connect(submitPatchJob, &ReviewBoard::SubmitPatchRequest::finished, this, &TheReviewboardJob::reviewDone);
        submitPatchJob->start();
    }

    void reviewDone(KJob* j)
    {
        if(j->error()!=0) {
            setError(j->error());
            setErrorText(j->errorString());
            emitResult();
            return;
        }

        ReviewBoard::SubmitPatchRequest const * job = qobject_cast<ReviewBoard::SubmitPatchRequest*>(j);
        QUrl url = job->server();
        url.setUserInfo(QString());
        QString requrl = QStringLiteral("%1/r/%2/").arg(url.toDisplayString(QUrl::PreferLocalFile), job->requestId());
        setOutput({{ QStringLiteral("url"), requrl }});
        emitResult();
    }
};

class Q_DECL_EXPORT ReviewBoardPlugin : public Purpose::PluginBase
{
    Q_OBJECT
    public:
        ReviewBoardPlugin(QObject* parent, const QList<QVariant>& /*args*/) : PluginBase(parent) {}
        virtual ~ReviewBoardPlugin() override {}

        virtual Purpose::Job* createJob() const override
        {
            return new TheReviewboardJob;
        }
};

K_PLUGIN_FACTORY_WITH_JSON(ReviewBoardPluginFactory, "reviewboardplugin.json", registerPlugin<ReviewBoardPlugin>();)

#include "reviewboardplugin.moc"
