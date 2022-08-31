/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef PURPOSE_PLUGINBASE_H
#define PURPOSE_PLUGINBASE_H

#include "job.h"
#include <kcoreaddons_export.h>

#include <QObject>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0) && KCOREADDONS_BUILD_DEPRECATED_SINCE(5, 84)
#define EXPORT_SHARE_VERSION K_EXPORT_PLUGIN_VERSION(2)
#else
// TODO KF6: remove, only added for backward compat in KF5
#define EXPORT_SHARE_VERSION
#endif

namespace Purpose
{
/**
 * @brief Base class to implement by plugins
 *
 * Class for Qt native plugins to extend. Plugins will just be issuing jobs
 * that will be configured then started.
 *
 * See README.md on how to create plugins on technologies different
 * to Qt.
 *
 * @note This file shouldn't be included by any application.
 */
class PURPOSE_EXPORT PluginBase : public QObject
{
    Q_OBJECT
public:
    explicit PluginBase(QObject *parent = nullptr);
    ~PluginBase() override;

    /** @returns the job that will perform the plugin's action. */
    virtual Job *createJob() const = 0;
};

}

Q_DECLARE_INTERFACE(Purpose::PluginBase, "org.kde.purpose")

#endif
