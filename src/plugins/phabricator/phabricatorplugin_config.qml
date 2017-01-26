/*
 Copyright 2015 Aleix Pol Gonzalez <aleixpol@kde.org>

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
    property string updateRR: ""
    property string baseDir
    property string localBaseDir
    property variant extraData: rcfile.extraData

    Label {
        text: root.updateRR
    }

    ReviewboardRC {
        id: json
        path: root.localBaseDir + "/.arcconfig"
    }

    function refreshUpdateRR()
    {
        root.updateRR = (updateRRCombo.currentIndex>=0 && update.checked) ? reviewsList.get(updateRRCombo.currentIndex, "toolTip") : ""
    }

    Item {
        Layout.fillWidth: true
        height: update.height

        CheckBox {
            anchors.centerIn: parent
            id: update
            text: i18n("Update Review:")
            enabled: updateRRCombo.count > 0
            onCheckedChanged: {
                root.refreshUpdateRR();
            }
        }
    }
    ComboBox {
        id: updateRRCombo
        Layout.fillWidth: true
        enabled: update.checked
        textRole: "display"
        model: ReviewsListModel {
            id: reviewsList
            status: "pending"
        }
        onCurrentIndexChanged: {
            root.refreshUpdateRR();
        }
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }
}
