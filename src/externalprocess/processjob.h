/*
 Copyright 2015 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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

#ifndef PURPOSE_PROCESSJOB_H
#define PURPOSE_PROCESSJOB_H

#include "job.h"
#include <QLocalServer>
#include <QLocalSocket>
#include <QJsonObject>
#include <QPointer>
#include <QProcess>

namespace Purpose
{

/**
 * @internal
 *
 * Purpose jobs can optionally run on an external process. This class interfaces
 * with the external process.
 */
class ProcessJob : public Job
{
Q_OBJECT
public:
    ProcessJob(const QString& pluginPath, const QString& pluginType, const QJsonObject& data, QObject* parent);
    ~ProcessJob();

    void start() override;

private:
    void writeSocket();
    void readSocket();
    void readAllSocket(bool ensureRead);
    void processStateChanged(QProcess::ProcessState state);

    QPointer<QProcess> m_process;

    QString m_pluginPath;
    QString m_pluginType;
    QJsonObject m_data;
    QLocalServer m_socket;
    QPointer<QLocalSocket> m_localSocket;
};

}

#endif
