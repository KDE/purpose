/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.purpose.accountshelper

ColumnLayout {
    id: root

    property alias videoDesc: description.text
    property alias videoTitle: title.text
    property alias videoTags: tags.text
    property var accountId
    property var urls
    property var mimeType

    function accountChanged() {
        accountId = accountsCombo.currentValue
        console.warn("val", accountId)
    }

    Label {
        text: i18nd("purpose6_youtube", "Account:")
    }
    RowLayout {
        Layout.fillWidth: true
        ComboBox {
            id: accountsCombo

            Layout.fillWidth: true
            textRole: "name"
            valueRole: "path"
            enabled: count > 0
            model: AccountsModel {
                id: accountsModel

                type: "google"
            }
            onCurrentIndexChanged: root.accountChanged()
            Component.onCompleted: root.accountChanged()
        }
        Button {
            icon.name: "settings-configure"
            onClicked: accountsModel.requestNew()
        }
    }

    Label {
        text: i18nd("purpose6_youtube", "Title:")
    }
    TextField {
        id: title
        Layout.fillWidth: true
        placeholderText: i18nd("purpose6_youtube", "Enter a title for the video...")
    }

    Label {
        text: i18nd("purpose6_youtube", "Tags:")
    }
    TextField {
        id: tags
        Layout.fillWidth: true
        placeholderText: i18nd("purpose6_youtube", "KDE, Kamoso")
    }

    Label {
        text: i18nd("purpose6_youtube", "Description:")
    }
    TextArea {
        id: description
        wrapMode: TextEdit.Wrap
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}
