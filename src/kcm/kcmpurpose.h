/*
    SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCMPURPOSE_H
#define KCMPURPOSE_H

#include <KQuickConfigModule>

#include <QDBusServiceWatcher>
#include <QNetworkAccessManager>

#include <memory>

class PluginModel;

class KCMPurpose : public KQuickConfigModule
{
    Q_OBJECT
    Q_PROPERTY(PluginModel *model READ model CONSTANT)

public:
    explicit KCMPurpose(QObject *parent, const KPluginMetaData &data, const QVariantList &args);
    ~KCMPurpose();

    void load() override;
    void save() override;

    void setDisabledPlugins(const QStringList &pluginIds);
    QStringList disabledPlugins() const;

    PluginModel *model() const;

Q_SIGNALS:
    void saveRequested();

private:
    QStringList readConfig() const;

    PluginModel *const m_model;
    QStringList m_disabledPlugins;
};

#endif // KCMPURPOSE_H
