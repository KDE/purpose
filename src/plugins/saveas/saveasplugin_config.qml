/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
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
