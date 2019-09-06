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

import QtQuick 2.7
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5
import org.kde.kirigami 2.5 as Kirigami
import org.kde.kdeconnect 1.0

ColumnLayout {

    id: root
    property string device: ""

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
            Layout.fillWidth: true
            Layout.fillHeight: true

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

            Label {
                anchors.fill: parent
                verticalAlignment: Qt.AlignVCenter
                horizontalAlignment: Qt.AlignHCenter
                visible: list.count === 0
                text: i18nd("purpose_kdeconnect","No devices found")
            }
        }
    }
}
