/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
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

    Layout.minimumHeight: item ? Math.max(item.Layout.minimumHeight, height) : height
    Layout.preferredHeight: item ? item.Layout.preferredHeight : width

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
