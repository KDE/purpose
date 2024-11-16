/*
    SPDX-FileCopyrightText: 2024 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "devicesmodel.h"

#include <QDBusPendingReply>
#include <QDBusServiceWatcher>
#include <QString>

#include "DaemonDbusInterface.h"
#include "DeviceDbusInterface.h"

DevicesModel::DevicesModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_daemonInterface(
          new OrgKdeKdeconnectDaemonInterface(QStringLiteral("org.kde.kdeconnect"), QStringLiteral("/modules/kdeconnect"), QDBusConnection::sessionBus(), this))
{
    connect(m_daemonInterface, &OrgKdeKdeconnectDaemonInterface::deviceAdded, this, &DevicesModel::deviceAdded);
    connect(m_daemonInterface, &OrgKdeKdeconnectDaemonInterface::deviceVisibilityChanged, this, &DevicesModel::deviceUpdated);
    connect(m_daemonInterface, &OrgKdeKdeconnectDaemonInterface::deviceRemoved, this, &DevicesModel::deviceRemoved);

    QDBusServiceWatcher *watcher =
        new QDBusServiceWatcher(QStringLiteral("org.kde.kdeconnect"), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this);
    connect(watcher, &QDBusServiceWatcher::serviceRegistered, this, &DevicesModel::loadDeviceList);
    connect(watcher, &QDBusServiceWatcher::serviceUnregistered, this, &DevicesModel::clearDevices);

    loadDeviceList();
}

QHash<int, QByteArray> DevicesModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractItemModel::roleNames();
    names.insert(NameModelRole, "name");
    names.insert(IconNameRole, "iconName");
    names.insert(IdRole, "deviceId");
    return names;
}

DevicesModel::~DevicesModel() = default;

int DevicesModel::rowForDevice(const QString &id) const
{
    for (int i = 0, c = m_devices.size(); i < c; ++i) {
        if (m_devices[i].id == id) {
            return i;
        }
    }
    return -1;
}

void DevicesModel::deviceAdded(const QString &id)
{
    if (rowForDevice(id) >= 0) {
        Q_ASSERT_X(false, "deviceAdded", "Trying to add a device twice");
        return;
    }

    auto dev = std::make_unique<OrgKdeKdeconnectDeviceInterface>(QStringLiteral("org.kde.kdeconnect"),
                                                                 QStringLiteral("/modules/kdeconnect/devices/") + id,
                                                                 QDBusConnection::sessionBus(),
                                                                 this);
    Q_ASSERT(dev->isValid());

    if (!dev->isPaired() || !dev->isReachable()) {
        return;
    }

    beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
    addDevice({id, std::move(dev)});
    endInsertRows();
}

void DevicesModel::deviceRemoved(const QString &id)
{
    int row = rowForDevice(id);
    if (row >= 0) {
        beginRemoveRows(QModelIndex(), row, row);
        m_devices.erase(m_devices.begin() + row);
        endRemoveRows();
    }
}

void DevicesModel::deviceUpdated(const QString &id)
{
    int row = rowForDevice(id);

    if (row < 0) {
        deviceAdded(id);
    } else {
        auto *dev = m_devices[row].interface.get();

        if (dev->isPaired() && dev->isReachable()) {
            const QModelIndex idx = index(row);
            Q_EMIT dataChanged(idx, idx);
        } else {
            beginRemoveRows(QModelIndex(), row, row);
            m_devices.erase(m_devices.begin() + row);
            endRemoveRows();
        }
    }
}

void DevicesModel::loadDeviceList()
{
    if (!m_daemonInterface->isValid()) {
        clearDevices();
        return;
    }

    QDBusPendingReply<QStringList> call = m_daemonInterface->devices(true /*onlyReachable*/, true /*onlyPaired*/);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

    QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, &DevicesModel::gotDeviceList);
}

void DevicesModel::gotDeviceList(QDBusPendingCallWatcher *watcher)
{
    watcher->deleteLater();
    clearDevices();
    QDBusPendingReply<QStringList> pendingDeviceIds = *watcher;
    if (pendingDeviceIds.isError()) {
        qWarning() << "Error while loading device list" << pendingDeviceIds.error().message();
        return;
    }

    Q_ASSERT(m_devices.empty());
    const QStringList deviceIds = pendingDeviceIds.value();

    if (deviceIds.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, deviceIds.count() - 1);
    for (const QString &id : deviceIds) {
        auto interface = std::make_unique<OrgKdeKdeconnectDeviceInterface>(QStringLiteral("org.kde.kdeconnect"),
                                                                           QStringLiteral("/modules/kdeconnect/devices/") + id,
                                                                           QDBusConnection::sessionBus(),
                                                                           this);

        if (interface->isPaired() && interface->isReachable()) {
            addDevice({id, std::move(interface)});
        }
    }
    endInsertRows();
}

void DevicesModel::addDevice(DeviceInterface &&dev)
{
    connect(dev.interface.get(), &OrgKdeKdeconnectDeviceInterface::nameChanged, this, [this, id = dev.id]() {
        Q_ASSERT(rowForDevice(id) >= 0);
        deviceUpdated(id);
    });
    m_devices.push_back(std::move(dev));
}

void DevicesModel::clearDevices()
{
    beginResetModel();
    m_devices.clear();
    endResetModel();
}

QVariant DevicesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= (int)m_devices.size()) {
        return QVariant();
    }

    OrgKdeKdeconnectDeviceInterface *device = m_devices[index.row()].interface.get();
    Q_ASSERT(device->isValid());

    switch (role) {
    case NameModelRole:
        return device->name();
    case IconNameRole:
        return device->statusIconName();
    case IdRole:
        return m_devices[index.row()].id;
    default:
        return QVariant();
    }
}

int DevicesModel::rowCount(const QModelIndex &) const
{
    return m_devices.size();
}

#include "moc_devicesmodel.cpp"
