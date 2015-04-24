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

import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.0
import QtQuick 2.0

ColumnLayout
{
    id: root
    property alias destinationPath: destination.text
    property variant urls

    Label {
        text: i18n("Save directory:")
    }
    Button {
        id: destination
        Layout.fillWidth: true

        iconName: "document-open-folder"
        onClicked: {
            dirSelector.visible = true
        }

        FileDialog {
            id: dirSelector
            title: i18n("Select a directory where to save your pictures and videos")
            selectMultiple: false
            selectExisting: true
            selectFolder: true

            onFileUrlChanged: {
                destination.text = dirSelector.fileUrl
            }
        }
    }
    Item {
        Layout.fillHeight: true
    }
}
