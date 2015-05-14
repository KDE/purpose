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
import org.kde.purpose 1.0
import org.kde.kquickcontrolsaddons 2.0

ApplicationWindow
{
    id: window

    width: 200
    height: 200

    property bool finished: false
    property var job
    property QtObject q

    Component.onCompleted: adoptJob()
    onJobChanged: adoptJob()

    function adoptJob() {
        if (job == null)
            return;
        if (job.isReady) {
            startJob()
        } else {
            view.push({
                item: configWizardComponent
            })
        }
    }

    function startJob() {
        window.job.start()
        view.push({
            item: runningJobComponent,
            properties: { job: window.job }
        })
    }

    StackView {
        id: view
        anchors.fill: parent
    }

    Component {
        id: configWizardComponent
        ColumnLayout {
            PurposeWizard {
                id: wiz
                job: window.job
                focus: true

                Layout.fillHeight: true
                Layout.fillWidth: true
            }
            RowLayout {
                Button {
                    text: i18n("Run")
                    enabled: window.job && window.job.isReady
                    onClicked: {
                        view.pop();
                        window.startJob();
                    }
                }
                Button {
                    text: i18n("Cancel")
                    onClicked: {
                        window.visible = false;
                    }
                }
            }
        }
    }

    Component {
        id: runningJobComponent
        RunningJob {
            onResult: {
                window.q.finished(output, window.job.error, window.job.errorString);
                window.visible = false
            }
        }
    }
}
