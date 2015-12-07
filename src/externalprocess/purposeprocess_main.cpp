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

#include <QApplication>
#include <KPluginMetaData>
#include <QCommandLineParser>
#include <QJsonDocument>
#include <QPointer>
#include <QLocalSocket>

#include <purpose/configuration.h>
#include <purpose/job.h>

class Communication : public QObject
{
Q_OBJECT
public:
    Communication(Purpose::Job* job, const QString &serverName)
        : m_job(job)
    {
        Q_ASSERT(job);
        connect(job, &KJob::finished, this, &Communication::finished);
        connect(job, SIGNAL(percent(KJob*, unsingned long)), this, SLOT(percent()));

        m_socket.setServerName(serverName);
        m_socket.connectToServer(QIODevice::WriteOnly);
        connect(&m_socket, SIGNAL(error(QLocalSocket::LocalSocketError)), this, SLOT(error()));

        bool b = m_socket.waitForConnected();
        Q_ASSERT(b);
    }

private Q_SLOTS:
    void error() {
        qWarning() << "socket error:" << m_socket.error();
    }
    void percent() {
        send({ {QStringLiteral("percent"), int(m_job->percent()) } });
    }

private:

    void finished() {
        send({ { QStringLiteral("output"), m_job->property("outputValues").toJsonObject() } });

//         QCoreApplication::instance()->exit(m_job->error());
    }

    void send(const QJsonObject &object) {
        const QByteArray data = QJsonDocument(object).toJson(QJsonDocument::Compact) + '\n';
        m_socket.write(data);
    }

    Purpose::Job* m_job;
    QLocalSocket m_socket;
};

int main(int argc, char** argv)
{
#warning make QGuiApplication, consider QCoreApplication?
    QApplication app(argc, argv);

    QJsonObject dataObject;
    QString pluginType;
    KPluginMetaData md;
    QString serverName;

    {
        QCommandLineParser parser;
        parser.addOption(QCommandLineOption(QStringLiteral("server"), QStringLiteral("Named socket to connect to"), QStringLiteral("name")));
        parser.addOption(QCommandLineOption(QStringLiteral("data"), QStringLiteral("See Purpose::Configuration::setData"), QStringLiteral("json")));
        parser.addOption(QCommandLineOption(QStringLiteral("pluginPath"), QStringLiteral("Chosen plugin"), QStringLiteral("path")));
        parser.addOption(QCommandLineOption(QStringLiteral("pluginType"), QStringLiteral("Plugin type name "), QStringLiteral("path")));
        parser.addHelpOption();

        parser.process(app);

        QJsonParseError error;
        const QJsonDocument doc = QJsonDocument::fromJson(parser.value(QStringLiteral("data")).toUtf8(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug() << "error: " << error.errorString();
            return 1;
        }
        dataObject = doc.object();
        serverName = parser.value(QStringLiteral("server"));
        md = KPluginMetaData(parser.value(QStringLiteral("pluginPath")));
        Q_ASSERT(md.isValid());
        pluginType = parser.value(QStringLiteral("pluginType"));
    }

    QPointer<Purpose::Configuration> config(new Purpose::Configuration(dataObject, pluginType, md));
    config->setUseSeparateProcess(false);
    config->setData(dataObject);

    Q_ASSERT(config->isReady());

    Purpose::Job* job = config->createJob();
    Communication c(job, serverName);
    job->start();

    return app.exec();
}

#include "purposeprocess_main.moc"
