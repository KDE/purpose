/*
    SPDX-FileCopyrightText: 2017 Lim Yuen Hoe <yuenhoe86@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.purpose.accountshelper

ColumnLayout {
    id: root

    property var folder: folderField.text
    property var accountId
    property var urls
    property var mimeType

    Kirigami.Heading {
        Layout.fillWidth: true
        text: i18nd("purpose6_nextcloud", "Select an account:")
        visible: list.count !== 0
        level: 1
        wrapMode: Text.Wrap
    }

    ScrollView {
        id: scroll

        Layout.fillWidth: true
        Layout.fillHeight: true

        Component.onCompleted: scroll.background.visible = true

        ListView {
            id: list

            clip: true

            model: AccountsModel {
                id: accountsModel

                type: "nextcloud"
            }

            delegate: ItemDelegate {
                required property string name
                required property string path
                required property string iconName

                width: ListView.view.width
                text: name
                icon.name: iconName
            }

            onCurrentIndexChanged: {
                if (currentIndex === -1) {
                    root.accountId = undefined
                    return
                }

                root.accountId = currentItem.path
            }

            Kirigami.PlaceholderMessage {
                anchors.centerIn: parent
                width: parent.width - (Kirigami.Units.largeSpacing * 4)
                visible: list.count === 0
                text: i18nd("purpose6_nextcloud", "No account configured")
            }
        }
    }

    Button {
        Layout.alignment: Qt.AlignRight

        text: i18nd("purpose6_nextcloud", "Add New Account")
        icon.name: "applications-internet"
        onClicked: accountsModel.requestNew()
    }

    Label {
        Layout.fillWidth: true
        text: i18nd("purpose6_nextcloud", "Upload to folder:")
    }

    TextField {
        id: folderField
        Layout.fillWidth: true
        text: "/"
        onTextChanged: {
            // Setting folder to undefined disables the Run button
            root.folder = text !== "" ? text : undefined
        }
    }
}
