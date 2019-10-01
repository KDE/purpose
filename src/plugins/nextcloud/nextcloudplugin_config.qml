/*
 Copyright 2017 Lim Yuen Hoe <yuenhoe86@gmail.com>

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
import org.kde.kquickcontrolsaddons 2.0 as KQCA
import Ubuntu.OnlineAccounts 0.1 as OA

ColumnLayout
{
    id: root

    property alias folder: folderField.text
    property var accountId
    property var urls
    property var mimeType

    function accountChanged()
    {
        var valid = accountsCombo.enabled && accountsCombo.currentIndex>=0;
        if (valid) { root.accountId = serviceModel.get(accountsCombo.currentIndex, "accountId"); }
        refreshConfigReady();
    }

    // without manually refreshing, auto-filled values don't seem to activate the "Run" button
    function refreshConfigReady()
    {
        var jobData = configuration.data;
        jobData['accountId'] = root.accountId;
        jobData['folder'] = root.folder;
        configuration.data = jobData;
    }

    Label { text: i18nd("purpose_nextcloud", "Account:") }
    RowLayout {
        Layout.fillWidth: true
        ComboBox {
            id: accountsCombo

            Layout.fillWidth: true
            textRole: "displayName"
            enabled: count>0
            model: OA.AccountServiceModel {
                id: serviceModel
                serviceType: "nextcloud-upload"
            }
            onCurrentIndexChanged: root.accountChanged()
            Component.onCompleted: root.accountChanged()
        }
        Button {
            iconName: "settings-configure"
            onClicked: KQCA.KCMShell.open("kcm_kaccounts");
        }
    }
    Label { text: i18nd("purpose_nextcloud", "Upload to folder:") }
    TextField {
        id: folderField
        Layout.fillWidth: true
        text: "/"
    }
}
