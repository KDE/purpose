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

#include "alternativesmodel.h"
#include <QDirIterator>
#include <QMimeType>
#include <QMimeDatabase>
#include <QList>
#include <QIcon>
#include <QDebug>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QJsonArray>
#include <QRegularExpression>
#include <QDBusConnectionInterface>
#include <QDBusConnection>

#include <KPluginLoader>
#include <KPluginMetaData>
#include <KSharedConfig>
#include <KConfigGroup>

#include "helper.h"
#include "configuration.h"
#include "job.h"

using namespace Purpose;

static const QStringList s_defaultDisabledPlugins = {QStringLiteral("saveasplugin")};

typedef bool (*matchFunction)(const QString& constraint, const QJsonValue& value);

static bool defaultMatch(const QString& constraint, const QJsonValue& value)
{
    return value == QJsonValue(constraint);
}

static bool mimeTypeMatch(const QString& constraint, const QJsonValue& value)
{
    if(value.isArray()) {
        foreach(const QJsonValue& val, value.toArray()) {
            if (mimeTypeMatch(constraint, val))
                return true;
        }
        return false;
    } else if(value.isObject()) {
        for(const QJsonValue& val : value.toObject()) {
            if (mimeTypeMatch(constraint, val))
                return true;
        }
        return false;
    } else if(constraint.contains(QLatin1Char('*'))) {
        return QRegExp(constraint, Qt::CaseInsensitive, QRegExp::Wildcard).exactMatch(value.toString());
    } else {
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForName(value.toString());
        return mime.inherits(constraint);
    }
}

static bool dbusMatch(const QString& constraint, const QJsonValue& value)
{
    Q_UNUSED(value)
    return QDBusConnection::sessionBus().interface()->isServiceRegistered(constraint);
}

static QMap<QString, matchFunction> s_matchFunctions = {
    { QStringLiteral("mimeType"), mimeTypeMatch },
    { QStringLiteral("dbus"), dbusMatch }
};

class Purpose::AlternativesModelPrivate
{
public:
    QVector<KPluginMetaData> m_plugins;
    QJsonObject m_inputData;
    QString m_pluginType;
    QStringList m_disabledPlugins = s_defaultDisabledPlugins;
    QJsonObject m_pluginTypeData;

    bool isPluginAcceptable(const KPluginMetaData &meta, const QStringList &disabledPlugins) const {
        const QJsonObject obj = meta.rawData();
        if(!obj.value(QStringLiteral("X-Purpose-PluginTypes")).toArray().contains(m_pluginType)) {
            qDebug() << "discarding" << meta.name() << meta.value(QStringLiteral("X-Purpose-PluginTypes"));
            return false;
        }

        if (disabledPlugins.contains(meta.pluginId()) || m_disabledPlugins.contains(meta.pluginId())) {
            //qDebug() << "disabled plugin" << meta.name() << meta.pluginId();
            return false;
        }

        const QJsonArray constraints = obj.value(QStringLiteral("X-Purpose-Constraints")).toArray();
        const QRegularExpression constraintRx(QStringLiteral("(\\w+):(.*)"));
        for(const QJsonValue& constraint: constraints) {
            Q_ASSERT(constraintRx.isValid());
            QRegularExpressionMatch match = constraintRx.match(constraint.toString());
            if (!match.isValid() || !match.hasMatch()) {
                qWarning() << "wrong constraint" << constraint.toString();
                continue;
            }
            QString propertyName = match.captured(1);
            QString constrainedValue = match.captured(2);
            bool acceptable = s_matchFunctions.value(propertyName, defaultMatch)(constrainedValue, m_inputData.value(propertyName));
            if (!acceptable) {
//                 qDebug() << "not accepted" << meta.name() << propertyName << constrainedValue << m_inputData[propertyName];
                return false;
            }
        }
        return true;
    }
};

AlternativesModel::AlternativesModel(QObject* parent)
    : QAbstractListModel(parent)
    , d_ptr(new AlternativesModelPrivate)
{
}

AlternativesModel::~AlternativesModel()
{
    Q_D(AlternativesModel);
    delete d;
}

QHash<int,QByteArray> AlternativesModel::roleNames() const
{
    QHash<int,QByteArray> roles = QAbstractListModel::roleNames();
    roles.unite({
        { IconNameRole, "iconName" },
        { PluginIdRole, "pluginId" }
    });
    return roles;
}

void AlternativesModel::setInputData(const QJsonObject &input)
{
    Q_D(AlternativesModel);
    if (input == d->m_inputData)
        return;

    d->m_inputData = input;
    initializeModel();

    Q_EMIT inputDataChanged();
}

void AlternativesModel::setPluginType(const QString& pluginType)
{
    Q_D(AlternativesModel);
    if (pluginType == d->m_pluginType)
        return;


    d->m_pluginTypeData = Purpose::readPluginType(pluginType);
    d->m_pluginType = pluginType;
    Q_ASSERT(d->m_pluginTypeData.isEmpty() == d->m_pluginType.isEmpty());

    initializeModel();

    Q_EMIT pluginTypeChanged();
}

QStringList AlternativesModel::disabledPlugins() const
{
    Q_D(const AlternativesModel);
    return d->m_disabledPlugins;
}

void AlternativesModel::setDisabledPlugins(const QStringList &pluginIds)
{
    Q_D(AlternativesModel);
    if (pluginIds == d->m_disabledPlugins)
        return;

    d->m_disabledPlugins = pluginIds;

    initializeModel();

    Q_EMIT disabledPluginsChanged();
}

QString AlternativesModel::pluginType() const
{
    Q_D(const AlternativesModel);
    return d->m_pluginType;
}

QJsonObject AlternativesModel::inputData() const
{
    Q_D(const AlternativesModel);
    return d->m_inputData;
}

Purpose::Configuration* AlternativesModel::configureJob(int row)
{
    Q_D(AlternativesModel);
    const KPluginMetaData pluginData = d->m_plugins.at(row);
    return new Configuration(d->m_inputData, d->m_pluginType, d->m_pluginTypeData, pluginData);
}

int AlternativesModel::rowCount(const QModelIndex& parent) const
{
    Q_D(const AlternativesModel);
    return parent.isValid() ? 0 : d->m_plugins.count();
}

QVariant AlternativesModel::data(const QModelIndex& index, int role) const
{
    Q_D(const AlternativesModel);
    if (!index.isValid() || index.row()>d->m_plugins.count())
        return QVariant();

    KPluginMetaData data = d->m_plugins[index.row()];
    switch (role) {
        case Qt::DisplayRole:
            return data.name();
        case Qt::ToolTip:
            return data.description();
        case IconNameRole:
            return data.iconName();
        case Qt::DecorationRole:
            return QIcon::fromTheme(data.iconName());
        case PluginIdRole:
            return data.pluginId();
    }
    return QVariant();
}

static QVector<KPluginMetaData> findScriptedPackages(std::function<bool(const KPluginMetaData &)> filter)
{
    QVector<KPluginMetaData> ret;
    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("kpackage/Purpose"), QStandardPaths::LocateDirectory);
    foreach(const QString &dir, dirs) {
        QDirIterator dirIt(dir, QDir::Dirs | QDir::NoDotAndDotDot);

        for(; dirIt.hasNext(); ) {
            QDir dir(dirIt.next());
            Q_ASSERT(dir.exists());
            if (!dir.exists(QStringLiteral("metadata.json")))
                continue;

            const KPluginMetaData info = Purpose::createMetaData(dir.absoluteFilePath(QStringLiteral("metadata.json")));
            if (filter(info)) {
                ret += info;
            }
        }
    }

    return ret;
}

void AlternativesModel::initializeModel()
{
    Q_D(AlternativesModel);
    if (d->m_pluginType.isEmpty()) {
        return;
    }

    const QJsonArray inbound = d->m_pluginTypeData.value(QStringLiteral("X-Purpose-InboundArguments")).toArray();
    foreach(const QJsonValue& arg, inbound) {
        if(!d->m_inputData.contains(arg.toString())) {
            qWarning() << "Cannot initialize model with data" << d->m_inputData << ". missing:" << arg;
            return;
        }
    }

    const auto config = KSharedConfig::openConfig(QStringLiteral("purposerc"));
    const auto group = config->group("plugins");
    const QStringList disabledPlugins = group.readEntry("disabled", QStringList());
    auto pluginAcceptable = [d, disabledPlugins](const KPluginMetaData& meta) {
        return d->isPluginAcceptable(meta, disabledPlugins);
    };

    beginResetModel();
    d->m_plugins = KPluginLoader::findPlugins(QStringLiteral("kf5/purpose"), pluginAcceptable);
    d->m_plugins += findScriptedPackages(pluginAcceptable);
    endResetModel();
}
