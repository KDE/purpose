/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import org.kde.purpose 1.0

ColumnLayout {
    id: root
    property alias job: conn.target

    signal result()

    Connections {
        id: conn
        onInfoMessage: {
            info.text = rich ? rich : plain
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
        to: 100
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
