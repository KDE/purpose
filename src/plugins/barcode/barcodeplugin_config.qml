/*
    SPDX-FileCopyrightText: 2021 Kai Uwe Broulik <kde@broulik.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick 2.7
import QtQuick.Window 2.12
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12 as QQC2
import org.kde.kirigami 2.12 as Kirigami

import org.kde.prison 1.0 as Prison

ColumnLayout {
    id: root

    property var urls: []
    property string mimeType
    property var dummy

    Component.onCompleted: {
        root.Window.window.minimumWidth = Qt.binding(function() {
            return Math.min(Kirigami.Units.gridUnit * 30,
                            root.implicitWidth + Kirigami.Units.gridUnit * 2);
        });
        root.Window.window.minimumHeight = Qt.binding(function() {
            return Math.min(Kirigami.Units.gridUnit * 30,
                            root.implicitHeight + Kirigami.Units.gridUnit * 4);
        });
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: barcodeItem.implicitWidth
        Layout.preferredHeight: barcodeItem.implicitHeight

        Prison.Barcode {
            id: barcodeItem
            readonly property bool valid: implicitWidth > 0 && implicitHeight > 0 && implicitWidth <= width && implicitHeight <= height
            anchors.fill: parent
            barcodeType: Prison.Barcode.QRCode
            // Cannot set visible to false as we need it to re-render when changing its size
            opacity: valid ? 1 : 0
            content: textField.text
        }

        QQC2.Label {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: i18nd("purpose_barcode", "Type a URL or some text to generate a QR code")
            wrapMode: Text.WordWrap
            visible: textField.length === 0
        }

        QQC2.Label {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: i18nd("purpose_barcode", "Creating QR code failed")
            wrapMode: Text.WordWrap
            visible: textField.length > 0 && barcodeItem.implicitWidth === 0 && barcodeItem.implicitHeight === 0
        }

        QQC2.Label {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: i18nd("purpose_barcode", "The QR code is too large to be displayed")
            wrapMode: Text.WordWrap
            visible: textField.length > 0 && (barcodeItem.implicitWidth > barcodeItem.width || barcodeItem.implicitHeight > barcodeItem.height)
        }
    }

    QQC2.TextField {
        id: textField
        Layout.fillWidth: true
        text: root.urls[0]
        // Random limit so it doesn't get too large
        maximumLength: 250
        placeholderText: i18nd("purpose_barcode", "Type a URL or some text...")
        Component.onCompleted: forceActiveFocus()
    }
}
