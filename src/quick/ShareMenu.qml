// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.purpose as Purpose

/**
 * @brief A menu that cascades and displays the different available sharing methods as sub menu items.
 *
 * @since 6.5
 */
QQC2.Menu {
    id: root

    /**
     * This property holds the input data for Purpose.
     *
     * @code{.qml}
     * Purpose.ShareMenu {
     *     inputData: {
     *         'urls': ['file://home/notroot/Pictures/mypicture.png'],
     *         'mimeType': ['image/png']
     *     }
     * }
     * @endcode
     */
    property alias inputData: alternativesModel.inputData

    /**
     * @brief The plugin type to provide to AlternativesModel.
     */
    property alias pluginType: alternativesModel.pluginType

    title: i18ndc("libpurpose6_quick", "@title:menu", "Share")
    icon.name: "emblem-shared-symbolic"

    enabled: alternativesRepeater.count > 0

    Repeater {
        id: alternativesRepeater

        model: Purpose.PurposeAlternativesModel {
            id: alternativesModel
            inputData: root.inputData
        }

        delegate: QQC2.MenuItem {
            required property var modelData
            required property int index

            text: modelData.display
            icon.name: modelData.iconName

            onTriggered: {
                const dialog = Qt.createComponent("org.kde.purpose", "ShareDialog").createObject(QQC2.Overlay.overlay, {
                    title: i18nd("libpurpose6_quick", "Share the selected media"),
                    index,
                    model: alternativesModel
                });
                dialog.open();
            }
        }
    }
}