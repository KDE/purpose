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

#define COLOURCODES "\u001B\[[0-9]*m"

using namespace Phabricator;

bool DifferentialRevision::buildArcCommand(const QString& workDir, const QString& patchFile)
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
            m_arcInput = patchFile;
        }
        m_arcCmd.setWorkingDirectory(workDir);
        connect(&m_arcCmd, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &DifferentialRevision::done);
        ret = true;
    } else {
        qCWarning(PLUGIN_PHABRICATOR) << "Could not find 'arc' in the path";
        setError(3);
        setErrorText(i18n("Could not find the 'arc' command"));
        setErrorString(errorText());
        ret = false;
    }
    return ret;
}

void DifferentialRevision::start()
{
    if (!m_arcCmd.program().isEmpty()) {
        qCDebug(PLUGIN_PHABRICATOR) << "starting" << m_arcCmd.program() << m_arcCmd.arguments();
        qCDebug(PLUGIN_PHABRICATOR) << "\twordDir=" << m_arcCmd.workingDirectory() << "stdin=" << m_arcInput;
        m_arcCmd.start();
    }
}

void DifferentialRevision::setErrorString(const QString& msg)
{
    QRegExp unwanted(QString::fromUtf8(COLOURCODES));
    m_errorString = msg;
    m_errorString.replace(unwanted, QString());
}

QString DifferentialRevision::scrubbedResult()
{
    QString result = QString::fromUtf8(m_arcCmd.readAllStandardOutput());
    // the return string can contain terminal text colour codes: remove them.
    QRegExp unwanted(QString::fromUtf8(COLOURCODES));
    result.replace(unwanted, QString());
    return result;
}

QStringList DifferentialRevision::scrubbedResultList()
{
    QStringList result = QString::fromUtf8(m_arcCmd.readAllStandardOutput()).split(QChar::LineFeed);
    // the return string can contain terminal text colour codes: remove them.
    QRegExp unwanted(QString::fromUtf8(COLOURCODES));
    result.replaceInStrings(unwanted, QString());
    // remove all (now) empty strings
    result.removeAll(QString());
    return result;
}


NewDiffRev::NewDiffRev(const QUrl& patch, const QString& projectPath, QObject* parent)
    : DifferentialRevision(QString(), parent)
    , m_patch(patch)
    , m_project(projectPath)
{
    buildArcCommand(projectPath, patch.toLocalFile());
}

void NewDiffRev::done(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode) {
        setError(exitCode);
        setErrorText(i18n("Could not create the new \"differential diff\""));
        setErrorString(QString::fromUtf8(m_arcCmd.readAllStandardError()));
        qCWarning(PLUGIN_PHABRICATOR) << "Could not create the new \"differential diff\":"
            << m_arcCmd.error() << ";" << errorString();
    } else {
        const QString stdout = scrubbedResult();
        const char *diffOpCode = "Diff URI: ";
        int diffOffset = stdout.indexOf(QLatin1String(diffOpCode));
        if (diffOffset >= 0) {
            m_diffURI = stdout.mid(diffOffset + strlen(diffOpCode)).split(QChar::LineFeed).at(0);
        } else {
            m_diffURI = stdout;
        }
    }

    emitResult();
}


UpdateDiffRev::UpdateDiffRev(const QUrl& patch, const QString& basedir,
                             const QString& id, const QString& updateComment, QObject* parent)
    : DifferentialRevision(id, parent)
    , m_patch(patch)
    , m_basedir(basedir)
{
    buildArcCommand(m_basedir, m_patch.toLocalFile());
    QStringList args = m_arcCmd.arguments();
    if (updateComment.isEmpty()) {
        args << QStringLiteral("--message")
            << QStringLiteral("<placeholder: patch updated via the purpose/phabricator plugin>");
    } else {
        args << QStringLiteral("--message") << updateComment;
    }
    m_arcCmd.setArguments(args);
}

void UpdateDiffRev::done(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode) {
        setError(exitCode);
        setErrorText(i18n("Patch upload to Phabricator failed"));
        setErrorString(QString::fromUtf8(m_arcCmd.readAllStandardError()));
        qCWarning(PLUGIN_PHABRICATOR) << "Patch upload to Phabricator failed with exit code"
            << exitCode << ", error" << m_arcCmd.error() << ";" << errorString();
    } else {
        const QString stdout = scrubbedResult();
        const char *diffOpCode = "Revision URI: ";
        int diffOffset = stdout.indexOf(QLatin1String(diffOpCode));
        if (diffOffset >= 0) {
            m_diffURI = stdout.mid(diffOffset + strlen(diffOpCode)).split(QChar::LineFeed).at(0);
        } else {
            m_diffURI = stdout;
        }
    }
    emitResult();
}


DiffRevList::DiffRevList(const QString& projectDir, QObject* parent)
    : DifferentialRevision(QString(), parent)
    , m_projectDir(projectDir)
{
    buildArcCommand(m_projectDir);
}

bool DiffRevList::buildArcCommand(const QString& workDir, const QString&)
{
    bool ret;
    QString arc = QStandardPaths::findExecutable(QStringLiteral("arc"));
    if (!arc.isEmpty()) {
        QStringList args;
        args << QStringLiteral("list");
        m_arcCmd.setProgram(arc);
        m_arcCmd.setArguments(args);
        m_arcCmd.setWorkingDirectory(workDir);
        connect(&m_arcCmd, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &DiffRevList::done);
        ret = true;
    } else {
        qCWarning(PLUGIN_PHABRICATOR) << "Could not find 'arc' in the path";
        setError(3);
        setErrorText(i18n("Could not find the 'arc' command"));
        setErrorString(errorText());
        ret = false;
    }
    return ret;
}

void DiffRevList::done(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode) {
        setError(exitCode);
        setErrorText(i18n("Could not get list of differential revisions"));
        setErrorString(QString::fromUtf8(m_arcCmd.readAllStandardError()));
        qCWarning(PLUGIN_PHABRICATOR) << "Could not get list of differential revisions"
            << m_arcCmd.error() << ";" << errorString();
    } else {
        QStringList reviews = scrubbedResultList();
        qCDebug(PLUGIN_PHABRICATOR) << "arc list returned:" << reviews;
        foreach (auto rev, reviews) {
            QRegExp revIDExpr(QString::fromUtf8(" D[0-9][0-9]*: "));
            int idStart = rev.indexOf(revIDExpr);
            if (idStart >= 0) {
                QString revID = rev.mid(idStart+1).split(QString::fromUtf8(": ")).at(0);
                QString revTitle = rev.section(revIDExpr, 1);
                m_reviews << qMakePair(revID, revTitle);
                m_revMap[revTitle] = revID;
            }
        }
    }
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

