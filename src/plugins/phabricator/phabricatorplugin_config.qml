/*
 Copyright 2017 René J.V. Bertin <rjvbertin@gmail.com>

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
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.kde.purpose.phabricator 1.0

ColumnLayout {
    id: root
    property string updateDR: ""
    property string baseDir
    property string localBaseDir

    Label {
        text: root.updateDR
    }

    PhabricatorRC {
        id: json
        path: root.localBaseDir + "/.arcconfig"
    }

    function refreshUpdateDR()
    {
        root.updateDR = (updateDRCombo.currentIndex>=0 && update.checked) ? diffList.get(updateDRCombo.currentIndex, "toolTip") : ""
    }

    Item {
        Layout.fillWidth: true
        height: update.height

        CheckBox {
            anchors.centerIn: parent
            id: update
            text: i18n("Update Diff:")
            enabled: updateDRCombo.count > 0
            onCheckedChanged: {
                root.refreshUpdateDR();
            }
        }
    }
    ComboBox {
        id: updateDRCombo
        Layout.fillWidth: true
        enabled: update.checked
        textRole: "display"
        model: DiffListModel {
            id: diffList
            status: "pending"
        }
        onCurrentIndexChanged: {
            root.refreshUpdateDR();
        }
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }
}
