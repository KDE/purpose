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

#include "phabricatorjobs.h"
#include "debug.h"

#include <QTimer>
#include <QProcess>
#include <QStandardPaths>
// #include <QFile>
// #include <QJsonDocument>
// #include <QMimeDatabase>
// #include <QMimeType>
// #include <QNetworkReply>
// #include <QNetworkRequest>
// #include <QUrlQuery>

#include <KLocalizedString>

using namespace Phabricator;

bool DifferentialRevision::buildArcCommand(const QString& patchFile)
{
    bool ret;
    QString arc = QStandardPaths::findExecutable(QStringLiteral("arc"));
    if (!arc.isEmpty()) {
        QStringList args;
        args << QStringLiteral("diff");
        if (m_id.isEmpty()) {
            // creating a new differential revision (review request)
            // the fact we skip "--create" means we'll be creating a new "differential diff"
            // which obliges the user to fill in the details we cannot provide through the plugin ATM.
            // TODO: grab the TARGET_GROUPS from .reviewboardrc and pass that via --reviewers
        } else {
            // updating an existing differential revision (review request)
            args << QStringLiteral("--update") << m_id;
        }
        args << QStringLiteral("--excuse") << QStringLiteral("patch submitted with the purpose/phabricator plugin")
            << QStringLiteral("--raw");
        m_arcCmd.setProgram(arc);
        m_arcCmd.setArguments(args);
        if (!patchFile.isEmpty()) {
            m_arcCmd.setStandardInputFile(patchFile);
        }
        connect(&m_arcCmd, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &DifferentialRevision::done);
        ret = true;
    } else {
        qCWarning(PLUGIN_PHABRICATOR) << "Could not find 'arc' in the path";
        setError(3);
        setErrorText(i18n("Could not find the 'arc' command"));
        ret = false;
    }
    return ret;
}

void DifferentialRevision::start()
{
    if (!m_arcCmd.program().isEmpty()) {
        m_arcCmd.start();
    }
}

NewDiffRev::NewDiffRev(const QString& projectPath, QObject* parent)
    : DifferentialRevision(QString(), parent), m_project(projectPath)
{
}

void NewDiffRev::done(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit) {
        qCDebug(PLUGIN_PHABRICATOR) << "Could not create the new \"differential diff\":" << m_arcCmd.error();
        setError(exitCode);
        setErrorText(i18n("Could not create the new \"differential diff\""));
    } else {
        const QString stdout = QLatin1String(m_arcCmd.readAllStandardOutput());
        const char *diffOpCode = "Diff URI: ";
        int diffOffset = stdout.indexOf(QLatin1String(diffOpCode));
        if (diffOffset >= 0) {
            m_diffURI = stdout.mid(diffOffset + strlen(diffOpCode)).split(QChar::LineSeparator).at(0);
        }
//         QVariant res = m_newreq->result();
//         setRequestId(res.toMap()[QStringLiteral("review_request")].toMap()[QStringLiteral("id")].toString());
//         Q_ASSERT(!requestId().isEmpty());
    }

    emitResult();
}


SubmitDiffRev::SubmitDiffRev(const QUrl& patch, const QString& basedir, const QString& id, QObject* parent)
    : DifferentialRevision(id, parent), m_patch(patch), m_basedir(basedir)
{
    buildArcCommand(m_patch.toLocalFile());
}

void SubmitDiffRev::done(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit) {
        qCWarning(PLUGIN_PHABRICATOR) << "Patch upload to Phabricator failed with exit code"
            << exitCode << ", error" << m_arcCmd.error();
        setError(exitCode);
        setErrorText(i18n("Patch upload to Phabricator failed"));
    }
    emitResult();
}

DiffRevList::DiffRevList(const QString& reviewStatus, QObject* parent)
    : DifferentialRevision(QString(), parent), m_reviewStatus(reviewStatus)
{
}

bool DiffRevList::buildArcCommand(const QString&)
{
    bool ret;
    QString arc = QStandardPaths::findExecutable(QStringLiteral("arc"));
    if (!arc.isEmpty()) {
        QStringList args;
        args << QStringLiteral("list");
        m_arcCmd.setProgram(arc);
        m_arcCmd.setArguments(args);
        connect(&m_arcCmd, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &DiffRevList::done);
        ret = true;
    } else {
        qCWarning(PLUGIN_PHABRICATOR) << "Could not find 'arc' in the path";
        setError(3);
        setErrorText(i18n("Could not find the 'arc' command"));
        ret = false;
    }
    return ret;
}

QVariantList DiffRevList::reviews() const
{
    return m_reviews;
}

void DiffRevList::requestReviewList(int startIndex)
{
//     QList<QPair<QString,QString> > reviewParameters;
// 
//     // In practice, the web API will return at most 200 repos per call, so just hardcode that value here
//     reviewParameters << qMakePair(QStringLiteral("max-results"), QStringLiteral("200"));
//     reviewParameters << qMakePair(QStringLiteral("start"), QString::number(startIndex));
//     reviewParameters << qMakePair(QStringLiteral("from-user"), m_user);
//     reviewParameters << qMakePair(QStringLiteral("status"), m_reviewStatus);
// 
//     HttpCall* reviewsCall = new HttpCall(m_server, QStringLiteral("/api/review-requests/"), reviewParameters, HttpCall::Get, QByteArray(), false, this);
//     connect(reviewsCall, &HttpCall::finished, this, &DiffRevList::done);
// 
//     reviewsCall->start();
}

void DiffRevList::done(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit) {
        qCDebug(PLUGIN_PHABRICATOR) << "Could not get list of differential revisions" << m_arcCmd.error();
        setError(exitCode);
        setErrorText(i18n("Could not get list of differential revisions"));
    }
    // TODO
    emitResult();
}

// UpdateDiffRev::UpdateDiffRev(const QUrl& server, const QString& id, const QVariantMap& newValues, QObject* parent)
//     : DifferentialRevision(server, id, parent)
// {
//     m_req = new HttpCall(this->server(), QStringLiteral("/api/review-requests/")+id+QStringLiteral("/draft/"), {}, HttpCall::Put, multipartFormData(newValues), true, this);
//     connect(m_req, &HttpCall::finished, this, &UpdateDiffRev::done);
// }
// 
// void UpdateDiffRev::start()
// {
//     m_req->start();
// }
// 
// void UpdateDiffRev::done()
// {
//     if (m_req->error()) {
//         qCWarning(PLUGIN_PHABRICATOR) << "Could not set all metadata to the review" << m_req->errorString() << m_req->property("result");
//         setError(3);
//         setErrorText(i18n("Could not set metadata"));
//     }
// 
//     emitResult();
// }

