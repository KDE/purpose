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
#include <QJsonArray>
#include <QDebug>

using namespace Purpose;

struct Purpose::JobPrivate
{
    QJsonObject m_data;
    QJsonArray configurationArguments;
    QJsonArray inboundArguments;
};

Job::Job(QObject* parent)
    : KJob(parent)
    , d_ptr(new JobPrivate)
{
}

Job::~Job()
{
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
    for(const QJsonValue& arg: neededArguments()) {
        if(!d->m_data.contains(arg.toString()))
            return false;
    }
    return true;
}

void Job::setInboundArguments(const QJsonValue& args)
{
    Q_D(Job);
    d->inboundArguments = args.toArray();
}

void Job::setConfigurationArguments(const QJsonValue& args)
{
    Q_D(Job);
    d->configurationArguments = args.toArray();
}

QJsonArray Job::neededArguments() const
{
    Q_D(const Job);
    QJsonArray ret = d->configurationArguments;
    foreach (const QJsonValue &val, d->inboundArguments)
        ret += val;
    return ret;
}
