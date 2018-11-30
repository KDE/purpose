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

import QtQuick 2.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.9
import org.kde.purpose 1.0

ApplicationWindow
{
    id: window

    flags: Qt.Dialog

    width: Math.max(screen.width/3, 200)
    height: Math.max(screen.height/2, 300)

    property bool finished: false
    property var configuration
    property QtObject q

    Component.onCompleted: adoptJob()
    onConfigurationChanged: adoptJob()
    minimumHeight: view.Layout.minimumHeight
    minimumWidth: view.Layout.minimumWidth

    function adoptJob() {
        if (configuration == null)
            return;
        if (configuration.isReady) {
            startJob()
        } else {
            view.push(configWizardComponent)
        }
    }

    function startJob() {
        var job = window.configuration.createJob();
        job.start()
        view.push(runningJobComponent, { job: job })
    }

    StackView {
        id: view
        anchors {
            fill: parent
            margins: 5
        }
        Layout.minimumWidth: (currentItem ? currentItem.Layout.minimumWidth : 0) + 2*anchors.margins
        Layout.minimumHeight: (currentItem ? currentItem.Layout.minimumHeight : 0) + 2*anchors.margins
    }

    Component {
        id: configWizardComponent
        Page {
            header: Label {
                TextMetrics {
                    id: fontMetrics
                }
                font.weight: Font.Light
                font.styleName: "Light"
                font.pointSize: fontMetrics.font.pointSize * 2
                elide: Text.ElideRight
                maximumLineCount: 1
                text: i18n("%1 - %2", window.configuration.pluginName, window.configuration.pluginTypeName)
            }
            PurposeWizard {
                id: wiz
                configuration: window.configuration
                focus: true

                anchors.fill: parent
            }

            footer: RowLayout {
                Button {
                    text: i18n("Run")
                    enabled: window.configuration && window.configuration.isReady
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
                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }

    Component {
        id: runningJobComponent
        RunningJob {
            onResult: {
                window.q.finished(job.output, job.error, job.errorString);
                window.visible = false
            }
        }
    }
}
