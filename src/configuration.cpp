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

#include "purpose/configuration.h"
#include <QFileInfo>
#include <KPluginFactory>
#include "externalprocess/processjob.h"

#include <KPluginLoader>
#include <KPluginMetaData>

#include <QDebug>
#include <QDir>
#include <QStandardPaths>

#include "helper.h"
#include "pluginbase.h"

using namespace Purpose;

class Purpose::ConfigurationPrivate
{
public:
    QJsonObject m_inputData;
    QString m_pluginTypeName;
    QJsonObject m_pluginType;
    const KPluginMetaData m_pluginData;
    bool m_useSeparateProcess;

    static void checkJobFinish(KJob* job)
    {
        QStringList outputArgs = job->property("outputArgs").toStringList();
        QJsonObject output = job->property("output").toJsonObject();

        if (!output.keys().toSet().contains(outputArgs.toSet()) && job->error() == 0) {
            qWarning() << "missing output values for" << job->metaObject()->className()
                       << ". Expected: " << outputArgs.join(QStringLiteral(", "))
                       << ". Got: " << output.keys().join(QStringLiteral(", "));
        }
    }

    Purpose::Job* internalCreateJob(QObject* parent) const {
        if (m_useSeparateProcess)
            return new ProcessJob(m_pluginData.fileName(), m_pluginTypeName, m_inputData, parent);
        else {
            return createJob(parent);
        }
    }

    Purpose::Job * createJob(QObject* parent) const
    {
        const QString fileName = m_pluginData.metaDataFileName();
        if(fileName.endsWith(QLatin1String("/metadata.json"))) {
            return new ProcessJob(m_pluginData.fileName(), m_pluginTypeName, m_inputData, parent);
        } else {
            KPluginLoader loader(fileName);
            KPluginFactory* factory = loader.factory();
            if (!factory) {
                qWarning() << "Couldn't create job:" << fileName << loader.errorString();
                return nullptr;
            }
            Purpose::PluginBase* plugin = dynamic_cast<Purpose::PluginBase*>(factory->create<QObject>(parent, QVariantList()));

            if (!plugin) {
                qWarning() << "Couldn't load plugin:" << fileName << loader.errorString();
                return nullptr;
            }

            return plugin->createJob();
        }
    }

};

Configuration::Configuration(const QJsonObject &inputData, const QString &pluginTypeName, const KPluginMetaData &pluginInformation, QObject* parent)
    : Configuration(inputData, pluginTypeName, QJsonObject(), pluginInformation, parent)
{}

Configuration::Configuration(const QJsonObject &inputData, const QString &pluginTypeName, const QJsonObject &pluginType, const KPluginMetaData &pluginInformation, QObject* parent)
    : QObject(parent)
    , d_ptr(new ConfigurationPrivate {inputData, pluginTypeName, pluginType, pluginInformation, !qEnvironmentVariableIsSet("KDE_PURPOSE_LOCAL_JOBS")})
{}

Configuration::~Configuration()
{
    delete d_ptr;
}

void Configuration::setData(const QJsonObject& data)
{
    Q_D(Configuration);

//     qDebug() << "datachanged" << data;
    if (d->m_inputData != data) {
        d->m_inputData = data;
        emit dataChanged();
    }
}

QJsonObject Configuration::data() const
{
    Q_D(const Configuration);
    return d->m_inputData;
}

bool Configuration::isReady() const
{
    Q_D(const Configuration);
    bool ok = true;
    Q_FOREACH(const QJsonValue& arg, neededArguments()) {
        if(!d->m_inputData.contains(arg.toString())) {
            qDebug() << "missing mandatory argument" << arg.toString();
            ok = false;
        }
    }
    return ok;
}

QJsonArray Configuration::neededArguments() const
{
    Q_D(const Configuration);
    QJsonArray ret = d->m_pluginType.value(QStringLiteral("X-Purpose-InboundArguments")).toArray();
    QJsonArray arr = d->m_pluginData.rawData().value(QStringLiteral("X-Purpose-Configuration")).toArray();
    Q_FOREACH(const QJsonValue &val, arr)
        ret += val;
    return ret;
}

Purpose::Job* Configuration::createJob()
{
    if (!isReady())
        return nullptr;

    Q_D(const Configuration);

    Purpose::Job* job = d->internalCreateJob(this);
    if (!job)
        return job;

    job->setData(d->m_inputData);
    job->setProperty("outputArgs", d->m_pluginType.value(QStringLiteral("X-Purpose-OutboundArguments")));

    connect(job, &Purpose::Job::finished, &ConfigurationPrivate::checkJobFinish);
    connect(job, &QObject::destroyed, this, &QObject::deleteLater);
    return job;
}

QUrl Configuration::configSourceCode() const
{
    Q_D(const Configuration);
    const QString metaDataPath = d->m_pluginData.metaDataFileName();
    if (metaDataPath.endsWith(QLatin1String("/metadata.json"))) {
        const QFileInfo fi(metaDataPath);
        return QUrl::fromLocalFile(fi.dir().filePath(QStringLiteral("contents/config/config.qml")));
    } else {
        const QString configFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("purpose/%1_config.qml").arg(d->m_pluginData.pluginId()));
        if (configFile.isEmpty())
            return QUrl();

        return QUrl::fromLocalFile(configFile);
    }
}

bool Configuration::useSeparateProcess() const
{
    Q_D(const Configuration);
    return d->m_useSeparateProcess;
}

void Configuration::setUseSeparateProcess(bool use)
{
    Q_D(Configuration);
    d->m_useSeparateProcess = use;
}

QString Configuration::pluginTypeName() const
{
    Q_D(const Configuration);
    KPluginMetaData md(d->m_pluginType, {});
    return md.name();
}

QString Configuration::pluginName() const
{
    Q_D(const Configuration);
    return d->m_pluginData.name();
}
