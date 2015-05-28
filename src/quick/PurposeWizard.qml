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

import QtQuick 2.2
import QtQuick.Layouts 1.0

Loader
{
    id: loader
    property QtObject configuration
    signal accepted()

    function cancel() {
        configuration.destroy();
    }

    Layout.minimumHeight: item ? item.Layout.minimumHeight : 0
    Layout.preferredHeight: item ? item.Layout.preferredHeight : 0

    Component.onCompleted: {
        setSource(configuration.configSourceCode, configuration.data)
    }
    onItemChanged: {
        for(var i in configuration.neededArguments) {
            var arg = configuration.neededArguments[i]
            if (arg in loader.item) {
                item[arg+"Changed"].connect(dataHasChanged);
            } else
                console.warn("property not found", arg);
        }
    }

    function dataHasChanged()
    {
        var jobData = configuration.data;
        for(var i in configuration.neededArguments) {
            var arg = configuration.neededArguments[i]
            if (arg in loader.item) {
                jobData[arg] = loader.item[arg];
            } else
                console.warn("property not found", arg);
        }
        configuration.data = jobData;
    }
}
