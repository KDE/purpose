/*
 SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

 SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import org.kde.purpose 1.0 as Purpose

ApplicationWindow
{
    id: window
    property alias inputData: view.inputData

    Purpose.AlternativesView {
        id: view
        anchors.fill: parent

        Layout.minimumWidth: 200
        Layout.minimumHeight: 200

        header: Label {
            text: {
                var ret = "Sharing "
                const urls = window.inputData.urls
                for (var u in urls)
                    ret += urls[u] + " ";
                return ret;
            }
        }

        pluginType: "Export"
        onFinished: {
            if (error != 0) {
                console.log("job finished with error", error, message)
            } else {
                console.log("Job finished:", output.url)
            }
        }
    }
}
