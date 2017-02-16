/*
 Copyright 2017 René J.V. Bertin <rjvbertin@gmail.com>

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

#ifndef PHABRICATORPURPOSEQUICKPLUGIN_H
#define PHABRICATORPURPOSEQUICKPLUGIN_H

#include <QQmlExtensionPlugin>

class PhabricatorPurposeQuickPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    virtual void registerTypes(const char* uri) Q_DECL_OVERRIDE;
};

#endif // PHABRICATORPURPOSEQUICKPLUGIN_H
