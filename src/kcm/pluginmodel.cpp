#include "pluginmodel.h"

#include "../helper.h"
#include "kcmpurpose.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QDirIterator>
#include <QStandardPaths>

// TODO: This is just duplicated from AlternativesModel
static QList<KPluginMetaData> findScriptedPackages()
{
    QList<KPluginMetaData> ret;
    QSet<QString> addedPlugins;
    const QStringList dirs =
        QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("kpackage/Purpose"), QStandardPaths::LocateDirectory);
    for (const QString &dir : dirs) {
        QDirIterator dirIt(dir, QDir::Dirs | QDir::NoDotAndDotDot);

        while (dirIt.hasNext()) {
            QDir dir(dirIt.next());
            Q_ASSERT(dir.exists());
            if (!dir.exists(QStringLiteral("metadata.json"))) {
                continue;
            }

            const KPluginMetaData info = Purpose::createMetaData(dir.absoluteFilePath(QStringLiteral("metadata.json")));
            if (!addedPlugins.contains(info.pluginId())) {
                addedPlugins << info.pluginId();
                ret += info;
            }
        }
    }

    return ret;
}

PluginModel::PluginModel(KCMPurpose *parent)
    : QAbstractListModel(parent)
    , m_kcm(parent)
{
    initializeModel();
}

int PluginModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_plugins.size();
}

QVariant PluginModel::data(const QModelIndex &index, int role) const
{
    const auto plugin = m_plugins[index.row()];
    const auto disabledPlugins = m_kcm->disabledPlugins();

    switch (role) {
    case Qt::DisplayRole:
    case Roles::Name:
        return plugin.name();
    case Roles::IconName:
        return plugin.iconName();
    case Roles::Description:
        return plugin.description();
    case Qt::CheckStateRole:
        return disabledPlugins.contains(plugin.pluginId()) ? Qt::Unchecked : Qt::Checked;
    }

    return {};
}

bool PluginModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const auto plugin = m_plugins[index.row()];
    auto disabledPlugins = m_kcm->disabledPlugins();
    if (value.toInt() == Qt::Checked) {
        disabledPlugins.removeAll(plugin.pluginId());
    } else if (value.toInt() == Qt::Unchecked) {
        disabledPlugins.append(plugin.pluginId());
    }
    m_kcm->setDisabledPlugins(disabledPlugins);
    Q_EMIT dataChanged(index, index, {role});
    return true;
}

QHash<int, QByteArray> PluginModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();

    roleNames.insert(Name, QByteArrayLiteral("name"));
    roleNames.insert(IconName, QByteArrayLiteral("iconName"));
    roleNames.insert(Description, QByteArrayLiteral("description"));
    roleNames.insert(Qt::CheckStateRole, QByteArrayLiteral("checkState"));

    return roleNames;
}

void PluginModel::initializeModel()
{
    beginResetModel();
    m_plugins.clear();
    m_plugins << KPluginMetaData::findPlugins(QStringLiteral("kf6/purpose"));
    m_plugins += findScriptedPackages();
    endResetModel();
}
