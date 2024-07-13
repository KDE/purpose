// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.purpose as Purpose
import org.kde.kirigami as Kirigami

/**
 * @brief An action that cascades and displays the different available sharing methods as sub actions.
 *
 * @since 6.10
 */
Kirigami.Action {
    id: shareAction
    icon.name: "emblem-shared-symbolic"
    text: i18nd("libpurpose6_quick", "Share")
    tooltip: i18nd("libpurpose6_quick", "Share the selected media")

    enabled: false

    /**
     * This property holds the input data for Purpose.
     *
     * @code{.qml}
     * Purpose.ShareAction {
     *     contentType: Purpose.ContentType.ShareUrl
     *     inputData: {
     *         'urls': ['file://home/notroot/Pictures/mypicture.png'],
     *         'mimeType': ['image/png']
     *     }
     * }
     * @endcode
     */
    property var inputData: ({})

    /**
     * @brief The content type to provide to AlternativesModel.
     */
    required property int contentType

    property Instantiator _instantiator: Instantiator {
        Component.onCompleted: {
            const purposeModel = Qt.createComponent("org.kde.purpose", "PurposeAlternativesModel").createObject(shareAction._instantiator);
            purposeModel.pluginType = Qt.binding(function() {
                switch(shareAction.contentType) {
                    case ContentType.ShareUrl:
                        return "ShareUrl";
                    case ContentType.Export:
                        return "Export";
                    default:
                        return "";
                }
            });
            purposeModel.inputData = Qt.binding(function() {
                return shareAction.inputData;
            });
            _instantiator.model = purposeModel;
            shareAction.enabled = true;
        }

        delegate: Kirigami.Action {
            property int index
            text: model.display ?? ""
            icon.name: model.iconName
            onTriggered: {
                const dialog = Qt.createComponent("org.kde.purpose", "ShareDialog").createObject(QQC2.Overlay.overlay, {
                    title: shareAction.tooltip,
                    index: index,
                    model: shareAction._instantiator.model
                });
                dialog.open();
            }
        }
        onObjectAdded: (index, object) => {
            object.index = index;
            shareAction.children.push(object)
        }
        onObjectRemoved: (index, object) => shareAction.children = Array.from(shareAction.children).filter(obj => obj.pluginId !== object.pluginId)
    }
}
