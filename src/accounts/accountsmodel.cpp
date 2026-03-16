/*
 *  SPDX-FileCopyrightText: 2025 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "accountsmodel.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QGuiApplication>
#include <QVariantMap>

#include <KIO/ApplicationLauncherJob>

#if HAVE_KACCOUNTS
#include <Accounts/Manager>
#include <KAccounts/Core>
#endif

#include "accounts_debug.h"

using namespace Qt::Literals;

bool usingKOnlineAccounts()
{
    return QDBusConnection::sessionBus().interface()->isServiceRegistered(u"org.kde.KOnlineAccounts"_s);
}

AccountsModel::AccountsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    if (usingKOnlineAccounts()) {
        // register
        QDBusMessage rm =
            QDBusMessage::createMethodCall(u"org.kde.KOnlineAccounts"_s, u"/org/kde/KOnlineAccounts"_s, u"org.kde.KOnlineAccounts.Manager"_s, u"registerApp"_s);

        rm.setArguments({qGuiApp->desktopFileName()});
        QDBusConnection::sessionBus().call(rm);

        // watch for granted accounts
        bool ret = QDBusConnection::sessionBus().connect(u"org.kde.KOnlineAccounts"_s,
                                                         u"/org/kde/KOnlineAccounts"_s,
                                                         u"org.kde.KOnlineAccounts.Manager"_s,
                                                         u"accountAccessGranted"_s,
                                                         this,
                                                         SLOT(slotAccountCreationFinished(const QDBusObjectPath &, const QString &)));
        Q_ASSERT(ret);
    } else {
#if HAVE_KACCOUNTS
        connect(KAccounts::accountsManager(), &Accounts::Manager::accountCreated, this, &AccountsModel::load);
        connect(KAccounts::accountsManager(), &Accounts::Manager::accountRemoved, this, &AccountsModel::load);
#endif
    }

    load();
}

void AccountsModel::load()
{
    m_accounts.clear();

    beginResetModel();

    if (usingKOnlineAccounts()) {
        QDBusMessage msg =
            QDBusMessage::createMethodCall(u"org.kde.KOnlineAccounts"_s, u"/org/kde/KOnlineAccounts"_s, u"org.freedesktop.DBus.Properties"_s, u"Get"_s);

        msg.setArguments({u"org.kde.KOnlineAccounts.Manager"_s, u"accounts"_s});

        QDBusReply<QDBusVariant> reply = QDBusConnection::sessionBus().call(msg);

        if (!reply.isValid()) {
            qCWarning(PURPOSE_ACCOUNTS_LOG) << "Error querying accounts:" << reply.error().message();
        }
        const QList<QDBusObjectPath> accounts = qdbus_cast<QList<QDBusObjectPath>>(reply.value().variant());

        for (const QDBusObjectPath &accountPath : accounts) {
            addFromDBus(accountPath);
        }

    } else {
#if HAVE_KACCOUNTS
        auto list = KAccounts::accountsManager()->accountList();

        for (auto id : list) {
            const auto account = KAccounts::accountsManager()->account(id);

            if (account->providerName() == m_type) {
                m_accounts.append({
                    .accountId = QString::number(id),
                    .name = account->displayName(),
                    .icon = account->provider().iconName(),
                });
            }
        }
#endif
    }

    endResetModel();
}

void AccountsModel::addFromDBus(const QDBusObjectPath &accountPath)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(u"org.kde.KOnlineAccounts"_s, accountPath.path(), u"org.freedesktop.DBus.Properties"_s, u"GetAll"_s);
    msg.setArguments({u"org.kde.KOnlineAccounts.Account"_s});
    QDBusReply<QVariantMap> reply = QDBusConnection::sessionBus().call(msg);
    if (!reply.isValid()) {
        qCWarning(PURPOSE_ACCOUNTS_LOG) << "Error reading properties for account" << accountPath << reply.error().message();
    }

    QVariantMap result = reply.value();

    const QString name = result[u"displayName"_s].toString();
    const QStringList types = result[u"types"_s].toStringList();
    const QString icon = result[u"icon"_s].toString();

    if (types.contains(m_type)) {
        m_accounts.append({
            .accountId = accountPath.path(),
            .name = name,
            .icon = icon,
        });
    }
}

QHash<int, QByteArray> AccountsModel::roleNames() const
{
    return {
        {Name, "name"},
        {AccountId, "accountId"},
        {IconName, "iconName"},
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
    case AccountId:
        return m_accounts[index.row()].accountId;
    case IconName:
        return m_accounts[index.row()].icon;
    }

    return QVariant();
}

void AccountsModel::requestNew()
{
    if (usingKOnlineAccounts()) {
        QDBusMessage m = QDBusMessage::createMethodCall(u"org.kde.KOnlineAccounts"_s,
                                                        u"/org/kde/KOnlineAccounts"_s,
                                                        u"org.kde.KOnlineAccounts.Manager"_s,
                                                        u"requestAccount"_s);

        m.setArguments({QStringList{m_type}, QString(/*TODO*/)});

        QDBusConnection::sessionBus().asyncCall(m);
    } else {
        auto job = new KIO::ApplicationLauncherJob(KService::serviceByDesktopName(u"kcm_kaccounts"_s));
        job->start();
    }
}

void AccountsModel::slotAccountCreationFinished(const QDBusObjectPath &path, const QString & /*xdgActivationToken*/)
{
    beginInsertRows({}, m_accounts.size(), m_accounts.size());
    addFromDBus(path);
    endInsertRows();
}

QString AccountsModel::type() const
{
    return m_type;
}

void AccountsModel::setType(const QString &type)
{
    if (type == m_type) {
        return;
    }

    m_type = type;
    Q_EMIT typeChanged();

    load();
}
