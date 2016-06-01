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
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.kde.kquickcontrolsaddons 2.0 as KQCA
import Ubuntu.OnlineAccounts 0.1 as OA

ColumnLayout
{
    id: root

    property alias videoDesc: description.text
    property alias videoTitle: title.text
    property alias videoTags: tags.text
    property var accountId
    property var urls
    property var mimeType

    function accountChanged()
    {
        var valid = accountsCombo.enabled && accountsCombo.currentIndex>=0;
        accountId = valid ? serviceModel.get(accountsCombo.currentIndex, "accountId") : null
    }

    Label { text: i18n("Account:") }
    RowLayout {
        Layout.fillWidth: true
        ComboBox {
            id: accountsCombo

            Layout.fillWidth: true
            textRole: "displayName"
            enabled: count>0
            model: OA.AccountServiceModel {
                id: serviceModel
                serviceType: "google-youtube"
            }
            onCurrentIndexChanged: root.accountChanged()
            Component.onCompleted: root.accountChanged()
        }
        Button {
            iconName: "settings-configure"
            onClicked: KQCA.KCMShell.open("kcm_kaccounts");
        }
    }

    Label { text: i18n("Title:") }
    TextField {
        id: title
        Layout.fillWidth: true
        placeholderText: i18n("Enter a title for the video...")
    }

    Label { text: i18n("Tags:") }
    TextField {
        id: tags
        Layout.fillWidth: true
        placeholderText: i18n("KDE, Kamoso")
    }

    Label { text: i18n("Description:") }
    TextArea {
        id: description
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}
