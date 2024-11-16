/*
    SPDX-FileCopyrightText: 2024 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef DEVICESMODEL_H
#define DEVICESMODEL_H

#include <QAbstractListModel>

#include <QQmlEngine>

#include <memory>
#include <vector>

class QDBusPendingCallWatcher;
class OrgKdeKdeconnectDaemonInterface;
class OrgKdeKdeconnectDeviceInterface;

class DevicesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum Roles {
        NameModelRole = Qt::DisplayRole,
        IconNameRole = Qt::DecorationRole,
        IdRole = Qt::UserRole + 1,
    };

    explicit DevicesModel(QObject *parent = nullptr);
    ~DevicesModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
    int rowForDevice(const QString &id) const;

private Q_SLOTS:
    void loadDeviceList();
    void gotDeviceList(QDBusPendingCallWatcher *watcher);

    void deviceAdded(const QString &id);
    void deviceRemoved(const QString &id);
    void deviceUpdated(const QString &id);

private:
    struct DeviceInterface {
        QString id;
        std::unique_ptr<OrgKdeKdeconnectDeviceInterface> interface;
    };

    void clearDevices();
    void addDevice(DeviceInterface &&dev);

    OrgKdeKdeconnectDaemonInterface *m_daemonInterface;
    std::vector<DeviceInterface> m_devices;
};

#endif
