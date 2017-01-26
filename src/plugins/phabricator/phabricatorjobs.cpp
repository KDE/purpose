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


SubmitPatchRequest::SubmitPatchRequest(const QUrl& patch, const QString& basedir, const QString& id, QObject* parent)
    : ReviewRequest(id, parent), m_patch(patch), m_basedir(basedir)
{
    QStringList args;
    QString arc = QStandardPaths::findExecutable(QLatin1String("arc"));
    if (!arc.isEmpty()) {
        args << QStringLiteral("diff") << QStringLiteral("--update") << requestId() << QStringLiteral("--raw");
        m_arcCmd.setProgram(arc);
        m_arcCmd.setArguments(args);
        m_arcCmd.setStandardInputFile(m_patch.toLocalFile());
        connect(&m_arcCmd, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), &SubmitPatchRequest::done);
    } else {
        qCWarning(PLUGIN_PHABRICATOR) << "Could not find 'arc' in the path";
        setError(3);
        setErrorText(i18n("Could not find the 'arc' command"));
    }
}

void SubmitPatchRequest::start()
{
    if (!m_arcCmd.program().isEmpty()) {
        m_arcCmd.start();
    }
}

void SubmitPatchRequest::done(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit) {
        qCWarning(PLUGIN_PHABRICATOR) << "Patch upload to Phabricator failed with exit code"
            << exitCode << ", error" << m_arcCmd.error();
        setError(exitCode);
        setErrorText(i18n("Patch upload to Phabricator failed"));
    }
    emitResult();
}

ProjectsListRequest::ProjectsListRequest(const QUrl& server, QObject* parent)
    : KJob(parent), m_server(server)
{
}

void ProjectsListRequest::start()
{
    requestRepositoryList(0);
}

QVariantList ProjectsListRequest::repositories() const
{
    return m_repositories;
}

void ProjectsListRequest::requestRepositoryList(int startIndex)
{
    QList<QPair<QString,QString> > repositoriesParameters;

    // In practice, the web API will return at most 200 repos per call, so just hardcode that value here
    repositoriesParameters << qMakePair(QStringLiteral("max-results"), QStringLiteral("200"));
    repositoriesParameters << qMakePair(QStringLiteral("start"), QString::number(startIndex));

    HttpCall* repositoriesCall = new HttpCall(m_server, QStringLiteral("/api/repositories/"), repositoriesParameters, HttpCall::Get, QByteArray(), false, this);
    connect(repositoriesCall, &HttpCall::finished, this, &ProjectsListRequest::done);

    repositoriesCall->start();
}

void ProjectsListRequest::done(KJob* job)
{
    // TODO error
    // TODO max iterations
    HttpCall* repositoriesCall = qobject_cast<HttpCall*>(job);
    QMap<QString, QVariant> resultMap = repositoriesCall->result().toMap();
    const int totalResults = repositoriesCall->result().toMap()[QStringLiteral("total_results")].toInt();
    m_repositories << repositoriesCall->result().toMap()[QStringLiteral("repositories")].toList();

    if (m_repositories.count() < totalResults) {
        requestRepositoryList(m_repositories.count());
    } else {
        emitResult();
    }
}

ReviewListRequest::ReviewListRequest(const QUrl& server, const QString& user, const QString& reviewStatus, QObject* parent)
    : KJob(parent), m_server(server), m_user(user), m_reviewStatus(reviewStatus)
{
}

void ReviewListRequest::start()
{
    requestReviewList(0);
}

QVariantList ReviewListRequest::reviews() const
{
    return m_reviews;
}

void ReviewListRequest::requestReviewList(int startIndex)
{
    QList<QPair<QString,QString> > reviewParameters;

    // In practice, the web API will return at most 200 repos per call, so just hardcode that value here
    reviewParameters << qMakePair(QStringLiteral("max-results"), QStringLiteral("200"));
    reviewParameters << qMakePair(QStringLiteral("start"), QString::number(startIndex));
    reviewParameters << qMakePair(QStringLiteral("from-user"), m_user);
    reviewParameters << qMakePair(QStringLiteral("status"), m_reviewStatus);

    HttpCall* reviewsCall = new HttpCall(m_server, QStringLiteral("/api/review-requests/"), reviewParameters, HttpCall::Get, QByteArray(), false, this);
    connect(reviewsCall, &HttpCall::finished, this, &ReviewListRequest::done);

    reviewsCall->start();
}

void ReviewListRequest::done(KJob* job)
{
    // TODO error
    // TODO max iterations
    if (job->error()) {
        qCDebug(PLUGIN_PHABRICATOR) << "Could not get reviews list" << job->errorString();
        setError(3);
        setErrorText(i18n("Could not get reviews list"));
        emitResult();
    }

    HttpCall* reviewsCall = qobject_cast<HttpCall*>(job);
    QMap<QString, QVariant> resultMap = reviewsCall->result().toMap();
    const int totalResults = resultMap[QStringLiteral("total_results")].toInt();

    m_reviews << resultMap[QStringLiteral("review_requests")].toList();

    if (m_reviews.count() < totalResults) {
        requestReviewList(m_reviews.count());
    } else {
        emitResult();
    }
}

UpdateRequest::UpdateRequest(const QUrl& server, const QString& id, const QVariantMap& newValues, QObject* parent)
    : ReviewRequest(server, id, parent)
{
    m_req = new HttpCall(this->server(), QStringLiteral("/api/review-requests/")+id+QStringLiteral("/draft/"), {}, HttpCall::Put, multipartFormData(newValues), true, this);
    connect(m_req, &HttpCall::finished, this, &UpdateRequest::done);
}

void UpdateRequest::start()
{
    m_req->start();
}

void UpdateRequest::done()
{
    if (m_req->error()) {
        qCWarning(PLUGIN_PHABRICATOR) << "Could not set all metadata to the review" << m_req->errorString() << m_req->property("result");
        setError(3);
        setErrorText(i18n("Could not set metadata"));
    }

    emitResult();
}

