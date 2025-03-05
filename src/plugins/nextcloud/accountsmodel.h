/*
 *  SPDX-FileCopyrightText: 2025 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include <QAbstractListModel>
#include <QDBusObjectPath>
#include <QHash>
#include <qqmlregistration.h>

class AccountsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    AccountsModel(QObject *parent = nullptr);

    enum Roles {
        Name = Qt::DisplayRole,
        Path = Qt::UserRole + 1,
    };

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    struct Data {
        QDBusObjectPath path;
        QString name;
    };

    QVector<Data> m_accounts;
};
