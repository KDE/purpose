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

#include "processjob.h"
#include <QMetaMethod>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QProcess>
#include <QStandardPaths>

using namespace Purpose;

ProcessJob::ProcessJob(const QString &pluginPath, const QString &pluginType, const QJsonObject& data, QObject* parent)
    : Job(parent)
    , m_process(new QProcess)
    , m_pluginPath(pluginPath)
    , m_pluginType(pluginType)
    , m_data(data)
    , m_localSocket(nullptr)
{
    QString exec = QStandardPaths::findExecutable(QStringLiteral("purposeprocess"), QStringList(QStringLiteral(CMAKE_INSTALL_FULL_LIBEXECDIR_KF5)));
    Q_ASSERT(!exec.isEmpty());
    m_process->setProgram(exec);
    m_process->setProcessChannelMode(QProcess::ForwardedChannels);

    connect(m_process, &QProcess::stateChanged, this, &ProcessJob::processStateChanged);

    m_socket.setSocketOptions(QLocalServer::UserAccessOption);
    bool b = m_socket.listen(QStringLiteral("caca"));
    Q_ASSERT(b);
    connect(&m_socket, &QLocalServer::newConnection, this, [this]() {
        m_localSocket = m_socket.nextPendingConnection();
        connect(m_localSocket, &QIODevice::readyRead, this, &ProcessJob::readSocket);
    });
}

void ProcessJob::readSocket()
{
    QJsonParseError error;
    const QMetaObject* mo = metaObject();
    while(m_localSocket->canReadLine()) {
        QByteArray json = m_localSocket->readLine();
        const QJsonObject object = QJsonDocument::fromJson(json, &error).object();
        if (error.error != QJsonParseError::NoError) {
            qWarning() << "error!" << error.errorString() << json;
            continue;
        }

        for(auto it=object.constBegin(), itEnd=object.constEnd(); it!=itEnd; ++it) {
            int idx = mo->indexOfProperty(it.key().toLatin1().constData());
            if (idx<0) {
                qWarning() << "unknown property" << it.key();
                continue;
            }

            QMetaProperty property = mo->property(idx);
            bool b = property.write(this, it.value());
            Q_ASSERT(b);
        }
    }
}

void ProcessJob::start()
{
    m_process->setArguments({
        QStringLiteral("--server"), m_socket.serverName(),
        QStringLiteral("--data"), QString::fromUtf8(QJsonDocument(m_data).toJson(QJsonDocument::Compact)),
        QStringLiteral("--pluginType"), m_pluginType,
        QStringLiteral("--pluginPath"), m_pluginPath
    });

//     qDebug() << "launching..." << m_process->program() << m_process->arguments();

    m_process->start();
}

void Purpose::ProcessJob::processStateChanged(QProcess::ProcessState state)
{
    if (state == QProcess::NotRunning) {
        Q_ASSERT(m_localSocket);
        readSocket();
        emitResult();
    }
}
