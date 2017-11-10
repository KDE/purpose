/*
 Copyright 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either 
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public 
 License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PURPOSE_PLUGINBASE_H
#define PURPOSE_PLUGINBASE_H

#include <QObject>
#include "job.h"

#define EXPORT_SHARE_VERSION K_EXPORT_PLUGIN_VERSION(2)

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
    PluginBase(QObject* parent = nullptr);
    ~PluginBase() override;

    /** @returns the job that will perform the plugin's action. */
    virtual Job* createJob() const = 0;
};

}

Q_DECLARE_INTERFACE(Purpose::PluginBase, "org.kde.purpose")

#endif
