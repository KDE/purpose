/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.7
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5
import org.kde.kirigami 2.12 as Kirigami

import org.kde.bluezqt 1.0 as BluezQt

ColumnLayout {

    id: root
    property var device: undefined

    anchors.bottomMargin: Kirigami.Units.smallSpacing

    Kirigami.Heading {
        text: i18nd("purpose_bluetooth", "Choose a device to send to:")
        visible: list.count !== 0
        level: 1
    }

    ScrollView {
        id: scroll

        Layout.fillWidth: true
        Layout.fillHeight: true

        // Make sure we have space to show the placeholdermessage when no explicit size is set
        contentHeight: list.count !== 0 ? implicitContentHeight : Kirigami.Units.gridUnit * 3

        Component.onCompleted: scroll.background.visible = true

        ListView {
            id: list

            // Don't select anything by default; make the user choose explicitly
            currentIndex: -1

            clip: true
            model: BluezQt.DevicesModel { }

            delegate: Kirigami.BasicListItem {
                width: parent.width
                text: Name
                icon: Icon
                onClicked: root.device = Ubi
                checked: root.device === Ubi
                highlighted: root.device === Ubi
            }

            Kirigami.PlaceholderMessage {
                anchors.centerIn: parent
                width: parent.width - (Kirigami.Units.largeSpacing * 4)
                visible: list.count === 0
                text: i18nd("purpose_bluetooth", "No devices found")
            }
        }
    }
}
