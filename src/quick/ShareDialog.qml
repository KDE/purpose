// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.purpose as Purpose
import org.kde.kirigami as Kirigami

/**
 * @brief The internal dialog used for sharing content. It's spawned via a ShareAction or ShareMenu when needed.
 *
 * @internal
 * @since 6.10
 */
Kirigami.Dialog {
    id: window

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    implicitWidth: Kirigami.Units.gridUnit * 30
    implicitHeight: Kirigami.Units.gridUnit * 20

    property alias index: jobView.index
    property alias model: jobView.model

    Component.onCompleted: jobView.start()

    contentItem: Purpose.JobView {
        id: jobView

        onStateChanged: {
            if (state === Purpose.PurposeJobController.Finished) {
                window.close()
            } else if (state === Purpose.PurposeJobController.Error) {
                window.close()
            } else if (state === Purpose.PurposeJobController.Cancelled) {
                // Do nothing
                window.close()
            }
        }
    }
}
