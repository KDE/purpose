/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.2
import QtQuick.Controls 2.15
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

    Label { text: i18nd("purpose_youtube", "Account:") }
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
            icon.name: "settings-configure"
            onClicked: KQCA.KCMShell.open("kcm_kaccounts");
        }
    }

    Label { text: i18nd("purpose_youtube", "Title:") }
    TextField {
        id: title
        Layout.fillWidth: true
        placeholderText: i18nd("purpose_youtube", "Enter a title for the video...")
    }

    Label { text: i18nd("purpose_youtube", "Tags:") }
    TextField {
        id: tags
        Layout.fillWidth: true
        placeholderText: i18nd("purpose_youtube", "KDE, Kamoso")
    }

    Label { text: i18nd("purpose_youtube", "Description:") }
    TextArea {
        id: description
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}
