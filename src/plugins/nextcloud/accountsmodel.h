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
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

public:
    AccountsModel(QObject *parent = nullptr);

    enum Roles {
        Name = Qt::DisplayRole,
        Path = Qt::UserRole + 1,
        IconName,
    };

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Q_INVOKABLE void requestNew();

    Q_SLOT void slotAccountCreationFinished(const QDBusObjectPath &path, const QString &xdgActivationToken);

    void addFromDBus(const QDBusObjectPath &path);

    QString type() const;
    void setType(const QString &type);
    Q_SIGNAL void typeChanged();

private:
    struct Data {
        QDBusObjectPath path;
        QString name;
        QString icon;
    };

    void load();

    QVector<Data> m_accounts;
    QString m_type;
};
