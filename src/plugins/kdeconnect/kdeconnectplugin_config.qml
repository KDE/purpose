/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.7
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5
import org.kde.kirigami 2.12 as Kirigami
import org.kde.kdeconnect 1.0

ColumnLayout {

    id: root
    property var device: undefined

    anchors.fill: parent
    anchors.bottomMargin: Kirigami.Units.smallSpacing

    Kirigami.Heading {
        text: i18nd("purpose_kdeconnect", "Choose a device to send to:")
        visible: list.count !== 0
        level: 1
    }

    ScrollView {
        id: scroll

        Layout.fillWidth: true
        Layout.fillHeight: true

        Component.onCompleted: scroll.background.visible = true

        ListView {
            id: list

            clip: true
            model: DevicesModel {
                id: connectDeviceModel
                displayFilter: DevicesModel.Paired | DevicesModel.Reachable
            }

            delegate: Kirigami.BasicListItem {
                width: parent.width
                text: model.display
                icon: model.iconName
                onClicked: root.device = deviceId
                checked: root.device === deviceId
                highlighted: root.device === deviceId
            }

            Kirigami.PlaceholderMessage {
                anchors.centerIn: parent
                width: parent.width - (Kirigami.Units.largeSpacing * 4)
                visible: list.count === 0
                text: i18nd("purpose_kdeconnect","No devices found")
            }
        }
    }
}
