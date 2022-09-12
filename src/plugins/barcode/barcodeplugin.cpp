/*
    SPDX-FileCopyrightText: 2021 Kai Uwe Broulik <kde@broulik.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <KPluginFactory>
#include <purpose/pluginbase.h>

class Q_DECL_EXPORT BarcodePlugin : public Purpose::PluginBase
{
    Q_OBJECT
public:
    BarcodePlugin(QObject *p, const QVariantList &)
        : Purpose::PluginBase(p)
    {
    }

    Purpose::Job *createJob() const override
    {
        // It's never supposed to go beyond the configuration page
        Q_UNREACHABLE();
        return nullptr;
    }
};

K_PLUGIN_CLASS_WITH_JSON(BarcodePlugin, "barcodeplugin.json")

#include "barcodeplugin.moc"
