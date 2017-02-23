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
    enabled: true
    property string updateDR: ""
    property string drTitle: ""
    property string baseDir
    property string localBaseDir
    property string updateComment: ""
    // This is a workaround for installs where the result dialog doesn't always appear
    // or doesn't always show the revision URL.
    property alias doBrowse: doBrowseCheck.checked

    Label {
        text: (updateDRCombo.currentIndex>=0 && update.checked)
            ? i18n("Update differential revision %1", updateDR)
            : i18n("Create new \"differential diff\"")
    }

    PhabricatorRC {
        id: json
        path: root.localBaseDir + "/.arcconfig"
    }

    function refreshUpdateDR()
    {
        if (updateDRCombo.currentIndex>=0 && update.checked) {
            root.updateDR = diffList.get(updateDRCombo.currentIndex, "toolTip")
            root.drTitle = diffList.get(updateDRCombo.currentIndex, "display")
        } else {
            root.updateDR =  ""
            root.drTitle = ""
        }
    }

    Item {
        Layout.fillWidth: true
        height: update.height

        CheckBox {
            anchors.centerIn: parent
            id: update
            text: i18n("Update Diff")
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
        Layout.fillWidth: true
        height: update.height

        CheckBox {
            id: doBrowseCheck
            anchors.centerIn: parent
            text: i18n("Open Diff in browser")
            enabled: true
        }
    }
    Label {
        text: i18n("Summary of the update to %1:", updateDR)
        enabled: update.checked
    }
    TextArea {
        id: updateCommentField
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: ""
        enabled: update.checked
        onEditingFinished: {
            root.updateComment = text
        }
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }
}
