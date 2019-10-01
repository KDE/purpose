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
    property string localBaseDir
    property alias updateComment: updateCommentField.text
    // This is a workaround for installs where the result dialog doesn't always appear
    // or doesn't always show the revision URL.
    property alias doBrowse: doBrowseCheck.checked
    property variant urls
    property variant mimeType

    function labelText()
    {
        if (updateDRCombo.currentIndex>=0 && updateOld.checked) {
            return updateDR.length > 0 ? i18nd("purpose_phabricator", "Update differential revision %1", updateDR) : i18nd("purpose_phabricator", "Update differential revision")
        } else if (createNew.checked) {
            return i18nd("purpose_phabricator", "Create new \"differential diff\"")
        } else {
            return i18nd("purpose_phabricator", "Create or update?")
        }
    }
    Label {
        id: label
        text: root.labelText()
    }

    PhabricatorRC {
        id: json
        path: root.localBaseDir + "/.arcconfig"
    }

    function refreshUpdateDR()
    {
        if (updateDRCombo.currentIndex>=0 && updateOld.checked) {
            root.updateDR = diffList.get(updateDRCombo.currentIndex, "toolTip")
            root.drTitle = diffList.get(updateDRCombo.currentIndex, "display")
        } else {
            if (createNew.checked) {
                root.updateDR = ""
                root.drTitle = ""
            } else {
                root.updateDR = i18nd("purpose_phabricator", "unknown")
                root.drTitle = ""
            }
        }
    }

    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        ExclusiveGroup {
            id: updateGroup
        }
        RadioButton {
            id: createNew
            exclusiveGroup: updateGroup
            text: i18nd("purpose_phabricator", "New Diff")
            tooltip: i18nd("purpose_phabricator", "tick this to create a new \"differential diff\" which can\nbe converted online to a new differential revision")
            onCheckedChanged: {
                root.refreshUpdateDR();
            }
        }
        RadioButton {
            id: updateOld
            exclusiveGroup: updateGroup
            text: i18nd("purpose_phabricator", "Update Diff")
            tooltip: i18nd("purpose_phabricator", "tick this to update an existing revision,\nselect one from the list below.")
            onCheckedChanged: {
                root.refreshUpdateDR();
            }
        }
    }

    ComboBox {
        id: updateDRCombo
        Layout.fillWidth: true
        enabled: updateOld.checked
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
        height: doBrowseCheck.height

        CheckBox {
            id: doBrowseCheck
            anchors.centerIn: parent
            text: i18nd("purpose_phabricator", "Open Diff in browser")
            enabled: updateOld.checked
        }
    }

    Label {
        // use i18nd("purpose_phabricator", ).arg() to avoid showing the "%1" when inactive
        text: updateDR != "unknown" && updateDR.length > 0 ? i18nd("purpose_phabricator", "Summary of the update to %1:", updateDR) : i18nd("purpose_phabricator", "Summary of the update")
        enabled: updateOld.checked
    }

    TextArea {
        id: updateCommentField
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: i18nd("purpose_phabricator", "patch updated through %1 and the Purpose/Phabricator plugin", Qt.application.name)
        enabled: updateOld.checked
        tabChangesFocus: false
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }
}
