/*
 *    SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "kcmpurpose.h"
#include "pluginmodel.h"

#include <KConfigGroup>
#include <KPluginFactory>
#include <KSharedConfig>

#include <QDirIterator>
#include <QHostInfo>
#include <QStandardPaths>

K_PLUGIN_CLASS_WITH_JSON(KCMPurpose, "kcm_purpose.json")

KCMPurpose::KCMPurpose(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : KQuickConfigModule(parent, data)
    , m_model(new PluginModel(this))
{
    qmlRegisterUncreatableType<PluginModel>("org.kde.purpose.kcm", 1, 0, "PluginModel", QStringLiteral("Access via kcm.model"));
}

KCMPurpose::~KCMPurpose() = default;

void KCMPurpose::load()
{
    m_disabledPlugins = readConfig();
}

void KCMPurpose::save()
{
    const auto config = KSharedConfig::openConfig(QStringLiteral("purposerc"));
    auto group = config->group(QStringLiteral("plugins"));
    group.writeEntry("disabled", m_disabledPlugins);

    Q_EMIT saveRequested();
    KQuickConfigModule::save();
}

void KCMPurpose::setDisabledPlugins(const QStringList &pluginIds)
{
    m_disabledPlugins = pluginIds;
    setNeedsSave(m_disabledPlugins != readConfig());
}

QStringList KCMPurpose::disabledPlugins() const
{
    return m_disabledPlugins;
}

PluginModel *KCMPurpose::model() const
{
    return m_model;
}

QStringList KCMPurpose::readConfig() const
{
    const auto config = KSharedConfig::openConfig(QStringLiteral("purposerc"));
    const auto group = config->group(QStringLiteral("plugins"));
    return group.readEntry("disabled", QStringList{QStringLiteral("saveasplugin")});
}

#include "kcmpurpose.moc"
#include "moc_kcmpurpose.cpp"
