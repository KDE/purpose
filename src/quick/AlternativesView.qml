/*
 Copyright 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import org.kde.purpose 1.0

StackView {
    id: stack
    property bool running: false
    property alias pluginType: altsModel.pluginType
    property alias inputData: altsModel.inputData
    property Component delegate: Component {
        RowLayout {
            width: parent.width
            Label {
                Layout.fillWidth: true
                text: display
                elide: Text.ElideRight
            }
            Button {
                text: i18n("Use")
                onClicked: createJob(index);
            }
        }
    }

    /**
     * Signals when the job finishes, reports the
     * @p error code and a text @p message.
     *
     * @p output will specify the output offered by the job
     */
    signal finished(var output, int error, string message)

    PurposeAlternativesModel {
        id: altsModel
    }

    function startJob(config) {
        var job = config.createJob();
        stack.push({
            item: runningJobComponent,
            properties: { job: job }
        })
        job.start()
        stack.running = true;
    }

    /**
     * Adopts the job at the @p index.
     */
    function createJob(index) {
        var conf = altsModel.configureJob(index);
        if (!conf.isReady) {
            stack.push({
                item: configWizardComponent,
                properties: { configuration: conf }
            })
        } else {
            startJob(conf)
        }
    }

    /**
     * Clears and returns back to the initial view.
     */
    function reset() {
        for(; stack.depth>1; stack.pop())
        {}
    }

    initialItem: ScrollView {
        ListView {
            model: altsModel
            delegate: stack.delegate
        }
    }

    Component {
        id: configWizardComponent
        ColumnLayout {
            property alias configuration: wiz.configuration
            ScrollView {
                id: scroll
                Layout.fillHeight: true
                Layout.fillWidth: true

                PurposeWizard {
                    id: wiz
                    focus: true
                    width: scroll.viewport.width
                }
            }
            RowLayout {
                Button {
                    text: i18n("Run")
                    enabled: wiz.configuration && wiz.configuration.isReady
                    onClicked: {
                        stack.pop();
                        startJob(wiz.configuration);
                    }
                }
                Button {
                    text: i18n("Back")
                    onClicked: {
                        stack.pop();
                        wiz.cancel()
                    }
                }
            }
        }
    }
    Component {
        id: runningJobComponent
        RunningJob {
            onResult: {
                stack.running = false;
                stack.finished(output, job.error, job.errorString);
                stack.pop();
            }
        }
    }
}
