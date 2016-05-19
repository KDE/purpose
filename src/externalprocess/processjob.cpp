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
#include <QLibrary>
#include <KRandom>
#include <QMetaMethod>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QProcess>
#include <QFile>
#include <QFileInfo>
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
    if (QLibrary::isLibrary(pluginPath)) {
        QString exec = QStandardPaths::findExecutable(QStringLiteral("purposeprocess"), QStringList(QStringLiteral(CMAKE_INSTALL_FULL_LIBEXECDIR_KF5)));
        Q_ASSERT(!exec.isEmpty());
        m_process->setProgram(exec);
    } else {
        Q_ASSERT(QFile::exists(pluginPath));
        Q_ASSERT(QFileInfo(pluginPath).permission(QFile::ExeOther | QFile::ExeGroup | QFile::ExeUser));
        m_process->setProgram(pluginPath);
    }
    m_process->setProcessChannelMode(QProcess::ForwardedChannels);

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    connect(m_process, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
        qWarning() << "error!" << error;
    } );
#endif
    connect(m_process, &QProcess::stateChanged, this, &ProcessJob::processStateChanged);

    m_socket.setMaxPendingConnections(1);
    m_socket.setSocketOptions(QLocalServer::UserAccessOption);
    bool b = m_socket.listen(QStringLiteral("randomname-%1").arg(KRandom::random()));
    Q_ASSERT(b);
    connect(&m_socket, &QLocalServer::newConnection, this, &ProcessJob::writeSocket);
}

ProcessJob::~ProcessJob()
{
    m_process->kill();
    delete m_process;
}

void ProcessJob::writeSocket()
{
    m_localSocket = m_socket.nextPendingConnection();
    connect(m_localSocket, &QIODevice::readyRead, this, &ProcessJob::readSocket);

    m_socket.removeServer(m_socket.serverName());

    const QByteArray data = QJsonDocument(m_data).toJson(QJsonDocument::Compact);
    m_localSocket->write(QByteArray::number(data.size()) + '\n');
    const auto ret = m_localSocket->write(data);
    Q_ASSERT(ret == data.size());
    m_localSocket->flush();
}

void ProcessJob::readSocket()
{
    QJsonParseError error;
    while(m_localSocket && m_localSocket->canReadLine()) {
        const QByteArray json = m_localSocket->readLine();

        const QJsonObject object = QJsonDocument::fromJson(json, &error).object();
        if (error.error != QJsonParseError::NoError) {
            qWarning() << "error!" << error.errorString() << json;
            continue;
        }

        for(auto it=object.constBegin(), itEnd=object.constEnd(); it!=itEnd; ++it) {
            const QByteArray propName = it.key().toLatin1();
            if (propName == "percent") {
                setPercent(it->toInt());
            } else if (propName == "error") {
                setError(it->toInt());
            } else if (propName == "errorText") {
                setErrorText(it->toString());
            } else if (propName == "output") {
                setOutput(it->toObject());
            }
        }
    }
}

void ProcessJob::start()
{
    m_process->setArguments({
        QStringLiteral("--server"), m_socket.fullServerName(),
        QStringLiteral("--pluginType"), m_pluginType,
        QStringLiteral("--pluginPath"), m_pluginPath
    });

    qDebug() << "launching..." << m_process->program() << m_process->arguments().join(QLatin1Char(' ')).constData();

    m_process->start();
}

void Purpose::ProcessJob::processStateChanged(QProcess::ProcessState state)
{
    if (state == QProcess::NotRunning) {
        Q_ASSERT(m_process->exitCode()!=0 || m_localSocket);
        if (m_process->exitCode()!=0) {
            qWarning() << "process exited with message:" << m_process->exitCode();
        }

        do {
            readSocket();
        } while (m_localSocket->waitForReadyRead());
        emitResult();
    }
}
