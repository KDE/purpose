/*
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.10
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.1

import org.kde.purpose 1.0
import org.kde.kirigami 2.10 as Kirigami

Item {

    property alias model: jobController.model
    property alias index: jobController.index

    readonly property alias state: jobController.state
    readonly property alias job: jobController.job

    function start() {
        jobController.configure()
    }

    PurposeJobController {
        id: jobController
    }

    Loader {
        anchors.fill: parent

        sourceComponent: {
            switch (jobController.state) {
                case PurposeJobController.Configuring: return configuring
                case PurposeJobController.Running: return running
                return undefined
            }
        }
    }

    Component {
        id: running

        Item {
            BusyIndicator {
                running: true
                anchors.centerIn: parent
            }
        }
    }

    Component {
        id: configuring

        Item {

            ColumnLayout {

                anchors.fill: parent
                anchors.leftMargin: Kirigami.Units.largeSpacing
                anchors.rightMargin: Kirigami.Units.largeSpacing
                anchors.topMargin: Kirigami.Units.largeSpacing
                anchors.bottomMargin: Kirigami.Units.largeSpacing

                Loader {
                    id: configLoader

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Component.onCompleted: setSource(jobController.configuration.configSourceCode, jobController.configuration.data)

                    onItemChanged: {
                        var initialData = jobController.configuration.data;
                        for(var i in jobController.configuration.neededArguments) {
                            var arg = jobController.configuration.neededArguments[i]
                            if (arg in configLoader.item) {
                                item[arg+"Changed"].connect(dataHasChanged);
                                initialData[arg] = item[arg];
                            } else {
                                console.warn("property not found", arg);
                            }
                        }
                        jobController.configuration.data = initialData;
                    }

                    function dataHasChanged()
                    {
                        var jobData = jobController.configuration.data;
                        for(var i in jobController.configuration.neededArguments) {
                            var arg = jobController.configuration.neededArguments[i]
                            if (arg in configLoader.item) {
                                jobData[arg] = configLoader.item[arg];
                            } else
                                console.warn("property not found", arg);
                        }
                        jobController.configuration.data = jobData;
                    }
                }

                RowLayout {
                    Button {
                        text: i18nd("libpurpose_quick", "Run")
                        enabled: jobController.configuration && jobController.configuration.isReady
                        onClicked: jobController.startJob()
                    }

                    Button {
                        text: i18nd("libpurpose_quick", "Back")
                        onClicked: jobController.cancel()
                    }
                }
            }
        }
    }
}
