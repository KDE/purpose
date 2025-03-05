/*
 *    SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KCMPLUGINMODEL_H
#define KCMPLUGINMODEL_H

#include <QAbstractItemModel>

#include <KPluginMetaData>

class KCMPurpose;

class PluginModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PluginModel(KCMPurpose *parent = nullptr);

    enum Roles {
        Name = Qt::UserRole,
        IconName,
        Description,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

private:
    void initializeModel();

    QList<KPluginMetaData> m_plugins;
    KCMPurpose *m_kcm;
};

#endif // KCMPLUGINMODEL_H
