/*
    SPDX-FileCopyrightText: 2015 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import org.kde.purpose 1.0

ApplicationWindow
{
    id: window

    flags: Qt.Dialog

    title: i18n("Send File")

    width: Math.max(screen.width/6, 200)
    height: Math.max(screen.height/4, 300)

    property QtObject q
    property alias model: jobView.model
    property alias index: jobView.index

    function start() {
        jobView.start()
    }

    JobView {
        id: jobView
        anchors.fill: parent

        onStateChanged: {
            if (state === PurposeJobController.Finished || state === PurposeJobController.Error) {
                console.log(jobView.job.errorString)
                window.q.finished(jobView.job.output, jobView.job.error, jobView.job.errorString);
                window.close()
            } else if (state === PurposeJobController.Cancelled) {
                window.q.finished(null, 1 /* KIO::ERR_USER_CANCELED */, i18nd("libpurpose_widgets", "Configuration cancelled"));
                window.close()
            }
        }
    }
}
