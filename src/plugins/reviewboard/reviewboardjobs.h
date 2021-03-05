/*
    SPDX-FileCopyrightText: 2010 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDEVPLATFORM_PLUGIN_REVIEWBOARDJOBS_H
#define KDEVPLATFORM_PLUGIN_REVIEWBOARDJOBS_H

#include "reviewboardhelpers_export.h"

#include <QList>
#include <QNetworkAccessManager>
#include <QPair>
#include <QUrl>

#include <KJob>

class QNetworkReply;

namespace ReviewBoard
{
/**
 * Http call to the specified service.
 * Converts returned json data to a QVariant to be used from actual API calls
 *
 * @note It is reviewboard-agnostic.
 */
class REVIEWBOARDHELPERS_EXPORT HttpCall : public KJob
{
    Q_OBJECT
    Q_PROPERTY(QVariant result READ result)
public:
    enum Method {
        Get,
        Put,
        Post,
    };

    HttpCall(const QUrl &s,
             const QString &apiPath,
             const QList<QPair<QString, QString>> &queryParameters,
             Method m,
             const QByteArray &post,
             bool multipart,
             QObject *parent);

    void start() override;

    QVariant result() const;

private Q_SLOTS:
    void onFinished();

private:
    // TODO: change to QJsonObject
    QVariant m_result;
    QNetworkReply *m_reply;
    QUrl m_requrl;
    QByteArray m_post;

    QNetworkAccessManager m_manager;
    bool m_multipart;
    Method m_method;
};

class REVIEWBOARDHELPERS_EXPORT ReviewRequest : public KJob
{
    Q_OBJECT
public:
    ReviewRequest(const QUrl &server, const QString &id, QObject *parent)
        : KJob(parent)
        , m_server(server)
        , m_id(id)
    {
    }
    QString requestId() const
    {
        return m_id;
    }
    void setRequestId(QString id)
    {
        m_id = id;
    }
    QUrl server() const
    {
        return m_server;
    }

private:
    QUrl m_server;
    QString m_id;
};

class REVIEWBOARDHELPERS_EXPORT NewRequest : public ReviewRequest
{
    Q_OBJECT
public:
    NewRequest(const QUrl &server, const QString &project, QObject *parent = nullptr);
    void start() override;

private Q_SLOTS:
    void done();

private:
    HttpCall *m_newreq;
    QString m_project;
};

class REVIEWBOARDHELPERS_EXPORT UpdateRequest : public ReviewRequest
{
    Q_OBJECT
public:
    UpdateRequest(const QUrl &server, const QString &id, const QVariantMap &newValues, QObject *parent = nullptr);
    void start() override;

private Q_SLOTS:
    void done();

private:
    HttpCall *m_req;
    QString m_project;
};

class REVIEWBOARDHELPERS_EXPORT SubmitPatchRequest : public ReviewRequest
{
    Q_OBJECT
public:
    SubmitPatchRequest(const QUrl &server, const QUrl &patch, const QString &basedir, const QString &id, QObject *parent = nullptr);
    void start() override;

private Q_SLOTS:
    void done();

private:
    HttpCall *m_uploadpatch;
    QUrl m_patch;
    QString m_basedir;
};

class REVIEWBOARDHELPERS_EXPORT ProjectsListRequest : public KJob
{
    Q_OBJECT
public:
    ProjectsListRequest(const QUrl &server, QObject *parent = nullptr);
    void start() override;
    QVariantList repositories() const;

private Q_SLOTS:
    void requestRepositoryList(int startIndex);
    void done(KJob *done);

private:
    QUrl m_server;
    QVariantList m_repositories;
};

class REVIEWBOARDHELPERS_EXPORT ReviewListRequest : public KJob
{
    Q_OBJECT
public:
    ReviewListRequest(const QUrl &server, const QString &user, const QString &reviewStatus, QObject *parent = nullptr);
    void start() override;
    QVariantList reviews() const;

private Q_SLOTS:
    void requestReviewList(int startIndex);
    void done(KJob *done);

private:
    QUrl m_server;
    QString m_user;
    QString m_reviewStatus;
    QVariantList m_reviews;
};

QByteArray urlToData(const QUrl &);
}

#endif
