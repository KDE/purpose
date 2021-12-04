/*
    SPDX-FileCopyrightText: 2017 Lim Yuen Hoe <yuenhoe86@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.2
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.1
import org.kde.kirigami 2.12 as Kirigami
import org.kde.kquickcontrolsaddons 2.0 as KQCA
//import Ubuntu.OnlineAccounts 0.1 as OA
import org.kde.kaccounts 1.2

ColumnLayout
{
    id: root

    property var folder: folderField.text
    property var accountId: 21
    property var urls
    property var mimeType

    Kirigami.Heading {
        text: i18nd("purpose_nextcloud", "Select an account:")
        visible: list.count !== 0
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
                id: serviceModel
//                 serviceType: "dav-storage"
                //provider: "dropbox"
            }

            delegate: Kirigami.BasicListItem {
                text: model.displayName
            }

            onCurrentIndexChanged: {
                if (currentIndex === -1) {
                    root.accountId = undefined
                    return
                }

                const idx = serviceModel.index(currentIndex, 0)

                root.accountId = serviceModel.data(idx, AccountsModel.IdRole)
            }

            Kirigami.PlaceholderMessage {
                anchors.centerIn: parent
                width: parent.width - (Kirigami.Units.largeSpacing * 4)
                visible: list.count === 0
                text: i18nd("purpose_nextcloud", "No account configured")
            }
        }
    }

    Button {
        Layout.alignment: Qt.AlignRight

        text: i18nd("purpose_nextcloud", "Configure Accounts")
        icon.name: "applications-internet"
        onClicked: KQCA.KCMShell.openSystemSettings("kcm_kaccounts")
    }

    Label {
        Layout.fillWidth: true
        text: i18nd("purpose_nextcloud", "Upload to folder:")
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
