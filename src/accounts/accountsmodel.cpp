/*
 *  SPDX-FileCopyrightText: 2025 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "accountsmodel.h"

#include <KIO/ApplicationLauncherJob>

#include <Accounts/Manager>
#include <KAccounts/Core>

using namespace Qt::Literals;

AccountsModel::AccountsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();

    connect(KAccounts::accountsManager(), &Accounts::Manager::accountCreated, this, &AccountsModel::load);
    connect(KAccounts::accountsManager(), &Accounts::Manager::accountRemoved, this, &AccountsModel::load);
}

void AccountsModel::load()
{
    m_accounts.clear();

    beginResetModel();

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

    endResetModel();
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
    auto job = new KIO::ApplicationLauncherJob(KService::serviceByDesktopName(u"kcm_kaccounts"_s));
    job->start();
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
