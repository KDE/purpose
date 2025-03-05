/*
 *  SPDX-FileCopyrightText: 2025 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "accountsmodel.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QVariantMap>

using namespace Qt::Literals;

AccountsModel::AccountsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QDBusMessage msg =
        QDBusMessage::createMethodCall(u"org.kde.KOnlineAccounts"_s, u"/org/kde/KOnlineAccounts"_s, u"org.freedesktop.DBus.Properties"_s, u"Get"_s);
    msg.setArguments({u"org.kde.KOnlineAccounts.Manager"_s, u"accounts"_s});
    QDBusReply<QVariant> reply = QDBusConnection::sessionBus().call(msg);
    qWarning() << "re" << reply.error();
    const auto accounts = qdbus_cast<QList<QDBusObjectPath>>(reply.value());

    beginResetModel();

    for (const QDBusObjectPath &accountPath : accounts) {
        QDBusMessage msg = QDBusMessage::createMethodCall(u"org.kde.KOnlineAccounts"_s, accountPath.path(), u"org.freedesktop.DBus.Properties"_s, u"GetAll"_s);
        msg.setArguments({u"org.kde.KOnlineAccounts.Account"_s});
        QDBusReply<QVariantMap> reply = QDBusConnection::sessionBus().call(msg);
        qWarning() << "re" << reply.error();

        QVariantMap result = reply.value();

        const QString name = result[u"displayName"_s].toString();
        const QStringList types = result[u"types"_s].toStringList();

        if (types.contains(u"nextcloud")) {
            m_accounts.append({
                .path = accountPath,
                .name = name,
            });
        }
    }

    endResetModel();
}

QHash<int, QByteArray> AccountsModel::roleNames() const
{
    return {
        {Name, "name"},
        {Path, "path"},
    };
}

int AccountsModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_accounts.size();
}

QVariant AccountsModel::data(const QModelIndex &index, int role) const
{
    switch (static_cast<Roles>(role)) {
    case Name:
        return m_accounts[index.row()].name;
    case Path:
        return m_accounts[index.row()].path.path();
    }

    return u"quack"_s;
}
