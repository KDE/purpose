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
#include <QHash>
#include <QPair>
#include <QUrl>

#include <KJob>
#include <QProcess>

class QNetworkReply;

namespace Phabricator
{
    class Q_DECL_EXPORT DifferentialRevision : public KJob
    {
        Q_OBJECT
        public:
            DifferentialRevision(const QString& id, QObject* parent)
                          : KJob(parent), m_id(id) {}
            QString requestId() const { return m_id; }
            void setRequestId(QString id) { m_id = id; }
            virtual void start() override;
            virtual QString errorString() const override
            {
                return m_errorString;
            }
            void setErrorString(const QString& msg);
            QString scrubbedResult();
            QStringList scrubbedResultList();

        private Q_SLOTS:
            virtual void done(int exitCode, QProcess::ExitStatus exitStatus) = 0;

        protected:
            virtual bool buildArcCommand(const QString& workDir, const QString& patchFile=QString());
            QProcess m_arcCmd;
        private:
            QString m_id;
            QString m_errorString;
            QString m_arcInput;
    };

    class Q_DECL_EXPORT NewDiffRev : public DifferentialRevision
    {
        Q_OBJECT
        public:
            NewDiffRev(const QUrl& patch, const QString& project, QObject* parent = 0);
            QString diffURI() const
            {
                return m_diffURI;
            }

        private Q_SLOTS:
            void done(int exitCode, QProcess::ExitStatus exitStatus) override;

        private:
            QUrl m_patch;
            QString m_project;
            QString m_diffURI;
    };

//     class Q_DECL_EXPORT UpdateDiffRev : public DifferentialRevision
//     {
//         Q_OBJECT
//         public:
//             UpdateDiffRev(const QString& id, const QVariantMap& newValues, QObject* parent = nullptr);
// 
//         private Q_SLOTS:
//             void done(int exitCode, QProcess::ExitStatus exitStatus) override;
// 
//         private:
//             QString m_project;
//     };

    class Q_DECL_EXPORT SubmitDiffRev : public DifferentialRevision
    {
        Q_OBJECT
        public:
            SubmitDiffRev(const QUrl& patch, const QString& basedir, const QString& id, QObject* parent = 0);
            QString diffURI() const
            {
                return m_diffURI;
            }

        private Q_SLOTS:
            void done(int exitCode, QProcess::ExitStatus exitStatus) override;

        private:
            QUrl m_patch;
            QString m_basedir;
            QString m_diffURI;
    };

    class Q_DECL_EXPORT DiffRevList : public DifferentialRevision
    {
        Q_OBJECT
        public:
            DiffRevList(const QString& projectDir, QObject* parent = 0);
            // return the open diff. revisions as a list of <diffID,diffDescription> pairs
            QList<QPair<QString,QString> > reviews() const
            {
                return m_reviews;
            }
            // return the open diff. revisions as a map of diffDescription->diffID entries
            QHash<QString,QString> reviewMap() const
            {
                return m_revMap;
            }

        private Q_SLOTS:
            void done(int exitCode, QProcess::ExitStatus exitStatus) override;

        protected:
            bool buildArcCommand(const QString& workDir, const QString& unused=QString()) override;
        private:
            QList<QPair<QString,QString> > m_reviews;
            QHash<QString,QString> m_revMap;
            QString m_projectDir;
    };
}

#endif
