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
import QtQuick.Controls 1.2
import org.kde.purpose 1.0

ColumnLayout {
    id: root
    property alias job: conn.target

    signal result()

    Connections {
        id: conn
        onInfoMessage: {
            info.text = rich
        }
        onResult: {
            root.result();
        }
    }
    Label {
        id: info

        Layout.fillWidth: true
        onLinkActivated: Qt.openUrlExternally(link)
    }
    ProgressBar {
        //FIXME: this is not really working yet, as QML doesn't understand ulong
        value: root.job.percent
        maximumValue: 100
        Layout.fillWidth: true
    }
    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }

//     TODO: Register KJob
//     Button {
//         anchors.right: parent.right
//         text: i18n("Cancel")
//         enabled: root.job && (root.job.capabilities & KJob.Killable)
//
//         onClicked: {
//             root.job.kill(KJob.EmitResult)
//         }
//     }
}
