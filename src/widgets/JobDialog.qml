/*
    SPDX-FileCopyrightText: 2015 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
    SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.purpose as Purpose
import org.kde.purpose.private.widgets as PurposeWidgets

QQC2.ApplicationWindow {
    id: window

    flags: Qt.Dialog
    title: i18n("Send File")
    visible: true

    width: Math.max(screen.width / 6, Kirigami.Units.gridUnit * 16)
    height: Math.max(screen.height / 4, Kirigami.Units.gridUnit * 20)

    minimumWidth: Kirigami.Units.gridUnit * 16
    minimumHeight: Kirigami.Units.gridUnit * 10

    property PurposeWidgets.Menu menu
    property Purpose.PurposeAlternativesModel model
    property int index

    function start(): void {
        jobView.start();
    }

    function cancel(): void {
        window.menu.finished({}, 1 /* KIO::ERR_USER_CANCELED */, i18nd("libpurpose6_widgets", "Configuration cancelled"));
        window.close();
    }

    onClosing: {
        if (jobView.state === Purpose.PurposeJobController.Configuring
            || jobView.state === Purpose.PurposeJobController.Running) {
            window.cancel();
        }
    }

    Purpose.JobView {
        id: jobView

        anchors.fill: parent

        model: window.model
        index: window.index

        onStateChanged: {
            switch (state) {
            case Purpose.PurposeJobController.Finished:
            case Purpose.PurposeJobController.Error:
                console.log(job.errorString);
                window.menu.finished(job.output, job.error, job.errorString);
                window.close();
                break;
            case Purpose.PurposeJobController.Cancelled:
                window.cancel();
                break;
            default:
                break;
            }
        }
    }
}
