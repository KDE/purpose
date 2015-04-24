/*
 Copyright 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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

#ifndef YOUTUBEJOBCOMPOSITE_H
#define YOUTUBEJOBCOMPOSITE_H

#include <purpose/job.h>

class YoutubeJobComposite : public Purpose::Job
{
public:
    YoutubeJobComposite();

    virtual void start() override;
    virtual QUrl configSourceCode() const;

private Q_SLOTS:
    void subjobFinished(KJob* subjob);

private:
    int m_pendingJobs;
};

#endif // YOUTUBEJOBCOMPOSITE_H
