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
            : Purpose::Job(parent)
        {}
        void start() override;

    private Q_SLOTS:
        void gotCredentials(KJob *job);
};
#endif /* NEXTCLOUDJOB_H */
