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
