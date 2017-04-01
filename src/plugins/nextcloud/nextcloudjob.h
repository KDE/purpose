/*
 Copyright 2017 Lim Yuen Hoe <yuenhoe86@gmail.com>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either 
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public 
 License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NEXTCLOUDJOB_H
#define NEXTCLOUDJOB_H

#include <purpose/pluginbase.h>
#include <QString>
#include <QUrl>

class NextcloudJob : public Purpose::Job
{
    Q_OBJECT
    public:
        NextcloudJob(QObject* parent)
            : Purpose::Job(parent), m_pendingJobs(0)
        {}
        void start() override;

    private Q_SLOTS:
        void fileUploaded(KJob*);
        void checkTargetFolder(KJob*);

    private:
        void checkTargetFile(const QUrl& local, KJob* job);
        void fileFetched(const QUrl& uploadUrl, KJob*);
        QUrl m_davUrl;
        int m_pendingJobs;
};
#endif /* NEXTCLOUDJOB_H */
