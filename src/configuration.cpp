/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2015  <copyright holder> <email>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include "purpose/configuration.h"

#include <KPluginMetaData>
#include <KPluginLoader>
#include <KPluginFactory>

#include <QDebug>
#include <QStandardPaths>

#include "pluginbase.h"

using namespace Purpose;

struct Purpose::ConfigurationPrivate
{
    QJsonObject m_inputData;
    QJsonObject m_pluginType;
    KPluginMetaData m_pluginData;

    static void checkJobFinish(KJob* job)
    {
        QStringList outputArgs = job->property("outputArgs").toStringList();
        QJsonObject output = job->property("outputValues").toJsonObject();

        if (!output.keys().toSet().contains(outputArgs.toSet()) && job->error() == 0) {
            qWarning() << "missing output values for" << job->metaObject()->className()
                       << ". Expected: " << outputArgs.join(QStringLiteral(", "))
                       << ". Got: " << output.keys().join(QStringLiteral(", "));
        }
    }
};

Configuration::Configuration(const QJsonObject &inputData, const QJsonObject &pluginType, const KPluginMetaData &pluginInformation, QObject* parent)
    : QObject(parent)
    , d_ptr(new ConfigurationPrivate {inputData, pluginType, pluginInformation})
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
    for(const QJsonValue& arg: neededArguments()) {
        if(!d->m_inputData.contains(arg.toString()))
            return false;
    }
    return true;
}

QJsonArray Configuration::neededArguments() const
{
    Q_D(const Configuration);
    QJsonArray ret = d->m_pluginType.value(QStringLiteral("X-Purpose-InboundArguments")).toArray();
    QJsonArray arr = d->m_pluginData.rawData().value(QStringLiteral("X-Purpose-Configuration")).toArray();
    foreach (const QJsonValue &val, arr)
        ret += val;
    return ret;
}

Purpose::Job* Configuration::createJob()
{
    if (!isReady())
        return Q_NULLPTR;

    Q_D(const Configuration);

    KPluginMetaData pluginData = d->m_pluginData;
    KPluginLoader loader(pluginData.fileName(), this);
    KPluginFactory* factory = loader.factory();
    if (!factory) {
        qWarning() << "Couldn't create job" << pluginData.fileName() << loader.errorString();
        return Q_NULLPTR;
    }
    Purpose::PluginBase* plugin = dynamic_cast<Purpose::PluginBase*>(factory->create<QObject>(this, QVariantList()));

    if (!plugin) {
        qWarning() << "Couldn't load plugin:" << pluginData.fileName() << loader.errorString();
    }

    Purpose::Job* job = plugin->share();
    job->setData(d->m_inputData);
    job->setProperty("outputArgs", d->m_pluginType.value(QStringLiteral("X-Purpose-OutboundArguments")));

    connect(job, &Purpose::Job::output, job, [job](const QJsonObject& obj){ job->setProperty("outputValues", obj); });
    connect(job, &Purpose::Job::finished, &ConfigurationPrivate::checkJobFinish);
    connect(job, &Purpose::Job::finished, this, &QObject::deleteLater);
    return job;
}

QUrl Configuration::configSourceCode() const
{
    Q_D(const Configuration);
    const QString configFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("purpose/%1_config.qml").arg(d->m_pluginData.pluginId()));
    if (configFile.isEmpty())
        return QUrl();

    return QUrl::fromLocalFile(configFile);
}
