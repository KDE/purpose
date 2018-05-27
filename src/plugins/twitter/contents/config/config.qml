/*
 Copyright 2015 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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

    property alias tweetText: tweet.text
    property string consumerKey: accts.authData.parameters.ConsumerKey
    property string consumerSecret: accts.authData.parameters.ConsumerSecret
    property var accessToken
    property var accessTokenSecret

    property var urls
    property var mimeType

    OA.AccountService {
        id: accts
        objectHandle: accountsCombo.enabled && accountsCombo.currentIndex>=0 ? serviceModel.get(accountsCombo.currentIndex, "accountServiceHandle") : null
        onAuthenticated: {
//             console.log("Access token is ", JSON.stringify(reply))
            root.accessToken = reply.AccessToken
            root.accessTokenSecret = reply.TokenSecret
        }
        onAuthenticationError: { console.log("Authentication failed, code " + error.code) }

        onAuthDataChanged: {
            authenticate(accts.authData);
        }
    }

    Label { text: i18nd("purpose_twitter", "Account:") }
    RowLayout {
        Layout.fillWidth: true
        ComboBox {
            id: accountsCombo

            Layout.fillWidth: true
            textRole: "displayName"
            enabled: count>0
            model: OA.AccountServiceModel {
                id: serviceModel
                serviceType: "twitter-microblog"
            }
        }
        Button {
            iconName: "settings-configure"
            onClicked: KQCA.KCMShell.open("kcm_kaccounts");
        }
    }

    TextField {
        id: tweet
        Layout.fillWidth: true
        Layout.fillHeight: true
        placeholderText: i18nd("purpose_twitter","What's happening?")
    }
}
