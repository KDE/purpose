// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.purpose as Purpose

Kirigami.ApplicationWindow {
    id: root

    title: "Purpose API Example"

    ColumnLayout {
        QQC2.Button {
            text: "Show Share Menu"

            onClicked: shareMenu.open()

            Purpose.ShareMenu {
                id: shareMenu

                contentType: Purpose.ContentType.ShareUrl
                inputData: {
                    'title': 'KDE',
                    'urls': ['https://kde.org/']
                }
            }
        }

        Kirigami.ActionToolBar {
            actions: Purpose.ShareAction {
                contentType: Purpose.ContentType.ShareUrl
                inputData: {
                    'title': 'KDE',
                    'urls': ['https://kde.org/']
                }
            }
        }
    }
}