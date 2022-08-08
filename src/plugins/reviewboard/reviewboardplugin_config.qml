/*
    SPDX-FileCopyrightText: 2015 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.2
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.1
import org.kde.purpose.reviewboard 1.0

ColumnLayout {
    id: root
    property alias server: serverField.text
    property alias username: usernameField.text
    property alias password: passwordField.text
    property alias repository: repositoriesCombo.currentText
    property string updateRR: ""
    property string baseDir
    property string localBaseDir
    property variant extraData: rcfile.extraData

    Label {
        text: root.updateRR
    }

    ReviewboardRC {
        id: rcfile
        path: root.localBaseDir + "/.reviewboardrc"
    }

    Label { text: i18nd("purpose_reviewboard", "Server:") }
    TextField {
        id: serverField
        Layout.fillWidth: true
        text: rcfile.server
    }
    Label { text: i18nd("purpose_reviewboard", "Username:") }
    TextField {
        id: usernameField
        Layout.fillWidth: true
    }
    Label { text: i18nd("purpose_reviewboard", "Password:") }
    TextField {
        id: passwordField
        echoMode: TextInput.Password
        Layout.fillWidth: true
    }

    Label { text: i18nd("purpose_reviewboard", "Repository:") }
    ComboBox {
        id: repositoriesCombo
        Layout.fillWidth: true
        textRole: "display"
        model: RepositoriesModel {
            server: serverField.text
            onRepositoriesChanged: {
                repositoriesCombo.currentIndex = findRepository(rcfile.repository);
            }
        }
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
            text: i18nd("purpose_reviewboard", "Update Review:")
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
            server: root.server
            repository: root.repository
            username: root.username
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
