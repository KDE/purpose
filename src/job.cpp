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

#include "job.h"
#include <QDebug>

using namespace Purpose;

struct Purpose::JobPrivate
{
    QJsonObject m_data;
    QStringList configurationArguments;
    QStringList inboundArguments;
};

Job::Job(QObject* parent)
    : KJob(parent)
    , d_ptr(new JobPrivate)
{
}

Job::~Job()
{
    delete d_ptr;
}

QJsonObject Job::data() const
{
    Q_D(const Job);
    return d->m_data;
}

void Job::setData(const QJsonObject& data)
{
    Q_D(Job);

//     qDebug() << "datachanged" << data;
    if (d->m_data != data) {
        d->m_data = data;
        emit dataChanged();
    }
}

bool Job::isReady() const
{
    Q_D(const Job);
    for(const QString& arg: neededArguments()) {
        if(!d->m_data.contains(arg))
            return false;
    }
    return true;
}

void Job::setInboundArguments(const QStringList& args)
{
    Q_D(Job);
    d->inboundArguments = args;
}

void Job::setConfigurationArguments(const QStringList& args)
{
    Q_D(Job);
    d->configurationArguments = args;
}

QStringList Job::neededArguments() const
{
    Q_D(const Job);
    return d->configurationArguments + d->inboundArguments;
}
