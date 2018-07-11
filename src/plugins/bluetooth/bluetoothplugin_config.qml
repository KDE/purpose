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

import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1

import org.kde.bluezqt 1.0 as BluezQt

ListView {
    id: root
    property string device: ""
    Layout.fillWidth: true
    Layout.fillHeight: true

    header: Label {
        text: i18n("Choose a device to send to:")
        visible: root.count !== 0
    }
    model: BluezQt.DevicesModel { }

    delegate: ItemDelegate {
        width: parent.width
        text: Name
        onClicked: root.device = Ubi
        checked: root.device === Ubi
        highlighted: root.device === Ubi
    }

    Label {
        anchors.fill: parent
        verticalAlignment: Qt.AlignVCenter
        horizontalAlignment: Qt.AlignHCenter
        visible: root.count === 0
        text: i18n("No devices found")
    }
}
