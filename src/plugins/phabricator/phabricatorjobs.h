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

#ifndef KDEVPLATFORM_PLUGIN_PHABRICATORJOBS_H
#define KDEVPLATFORM_PLUGIN_PHABRICATORJOBS_H

#include <QList>
#include <QNetworkAccessManager>
#include <QPair>
#include <QUrl>

#include <KJob>
#include <QProcess>

class QNetworkReply;

namespace Phabricator
{
    class Q_DECL_EXPORT ReviewRequest : public KJob
    {
        Q_OBJECT
        public:
            ReviewRequest(const QString& id, QObject* parent)
                          : KJob(parent), m_id(id) {}
            QString requestId() const { return m_id; }
            void setRequestId(QString id) { m_id = id; }

    protected:
            QProcess m_arcCmd;
        private:
            QString m_id;
    };

    class Q_DECL_EXPORT NewRequest : public ReviewRequest
    {
        Q_OBJECT
        public:
            NewRequest(const QString& project, QObject* parent = 0);
            virtual void start() override;

        private Q_SLOTS:
            void done();

        private:
            QString m_project;
    };

    class Q_DECL_EXPORT UpdateRequest : public ReviewRequest
    {
        Q_OBJECT
        public:
            UpdateRequest(const QString& id, const QVariantMap& newValues, QObject* parent = nullptr);
            virtual void start() override;

        private Q_SLOTS:
            void done();

        private:
            QString m_project;
    };

    class Q_DECL_EXPORT SubmitPatchRequest : public ReviewRequest
    {
        Q_OBJECT
        public:
            SubmitPatchRequest(const QUrl& patch, const QString& basedir, const QString& id, QObject* parent);
            virtual void start() override;

        private Q_SLOTS:
            void done(int exitCode, QProcess::ExitStatus exitStatus);

        private:
            QUrl m_patch;
            QString m_basedir;
    };

    class Q_DECL_EXPORT ProjectsListRequest : public KJob
    {
        Q_OBJECT
        public:
            ProjectsListRequest(QObject* parent = 0);
            virtual void start() override;
            QVariantList repositories() const;

        private Q_SLOTS:
            void requestRepositoryList(int startIndex);
            void done(KJob* done);

        private:
            QVariantList m_repositories;
    };

    class Q_DECL_EXPORT ReviewListRequest : public KJob
    {
        Q_OBJECT
        public:
            ReviewListRequest(const QString& user, const QString& reviewStatus, QObject* parent = 0);
            virtual void start() override;
            QVariantList reviews() const;

        private Q_SLOTS:
            void requestReviewList(int startIndex);
            void done(KJob* done);

        private:
            QString m_reviewStatus;
            QVariantList m_reviews;
    };

    QByteArray urlToData(const QUrl&);
}

#endif
