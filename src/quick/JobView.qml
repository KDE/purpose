/*
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.purpose
import org.kde.kirigami as Kirigami
import org.kde.ki18n
import org.kde.coreaddons as KCoreAddons

/*!
  \qmltype JobView
  \inqmlmodule org.kde.purpose

  JobView
*/
Item {

    /*!
      \qmlproperty AlternativesModel JobView::model
     */
    property alias model: jobController.model

    /*!
      \qmlproperty int JobView::index
     */
    property alias index: jobController.index

    /*!
      \qmlproperty enumeration JobView::state
     */
    readonly property alias jobState: jobController.state

    /*!
      \qmlproperty enumeration JobView::state
      \deprecated[6.27]
      Use jobState
     */
    @Deprecated { reason: "Use jobState instead" }
    readonly property alias state: jobController.state // qmllint disable property-override

    /*!
      \qmlproperty Job JobView::job
     */
    readonly property alias job: jobController.job

    implicitHeight: mainLoader.implicitHeight

    function start() {
        jobController.configure()
    }

    function formatElapsedTime(milliseconds) {
        return KCoreAddons.Format.formatDuration(milliseconds, KCoreAddons.FormatTypes.FoldHours)
    }

    function formatRemainingTime(elapsedMilliseconds, percent) {
        if (percent <= 0) {
            return ""
        }
        if (percent >= 100) {
            return formatElapsedTime(0)
        }
        return formatElapsedTime(elapsedMilliseconds * (100 - percent) / percent)
    }

    KI18nContext {
        id: _tr
        translationDomain: "libpurpose6_quick"
    }

    PurposeJobController {
        id: jobController
    }

    Loader {
        id: mainLoader
        anchors.fill: parent

        sourceComponent: {
            switch (jobController.state) {
            case PurposeJobController.Configuring:
                return configuring;
            case PurposeJobController.Running:
                return running;
            default:
                return null;
            }
        }
    }

    Component {
        id: running

        Item {
            id: runningRoot
            anchors.fill: parent

            property int elapsedTime: 0

            Timer {
                interval: 1000
                running: jobController.state === PurposeJobController.Running
                    && (!jobController.job || jobController.job.percent < 100)
                repeat: true
                triggeredOnStart: true
                onTriggered: runningRoot.elapsedTime = jobController.elapsedTime()
            }

            ColumnLayout {
                anchors.centerIn: parent
                width: Math.min(parent.width - Kirigami.Units.largeSpacing * 4, Kirigami.Units.gridUnit * 16)
                spacing: Kirigami.Units.smallSpacing

                ProgressBar {
                    Layout.fillWidth: true
                    from: 0
                    to: 100
                    value: jobController.job ? jobController.job.percent : 0
                    indeterminate: !jobController.job || jobController.job.percent === 0
                }

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        Layout.fillWidth: true
                        horizontalAlignment: runningRoot.elapsedTime > 0 ? Text.AlignLeft : Text.AlignHCenter
                        text: jobController.runningLabel || _tr.i18n("Sending…")
                    }

                    Label {
                        visible: runningRoot.elapsedTime > 0
                        opacity: 0.7
                        font: Kirigami.Theme.smallFont
                        text: {
                            const percent = jobController.job ? jobController.job.percent : 0
                            const remaining = formatRemainingTime(runningRoot.elapsedTime, percent)
                            return remaining.length > 0 ? remaining : formatElapsedTime(runningRoot.elapsedTime)
                        }
                    }
                }
            }
        }
    }

    Component {
        id: configuring

        ColumnLayout {
            Loader {
                id: configLoader

                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.largeSpacing

                Layout.fillHeight: true
                Layout.fillWidth: true

                Component.onCompleted: setSource(jobController.configuration.configSourceCode, jobController.configuration.data)

                onItemChanged: {
                    var initialData = jobController.configuration.data;
                    for (const i in jobController.configuration.neededArguments) {
                        const arg = jobController.configuration.neededArguments[i];
                        if (arg in configLoader.item) {
                            item[arg + "Changed"].connect(dataHasChanged);
                            initialData[arg] = item[arg];
                        } else {
                            console.warn("property not found", arg);
                        }
                    }
                    jobController.configuration.data = initialData;
                }

                function dataHasChanged() {
                    var jobData = jobController.configuration.data;
                    for (const i in jobController.configuration.neededArguments) {
                        const arg = jobController.configuration.neededArguments[i];
                        if (arg in configLoader.item) {
                            jobData[arg] = configLoader.item[arg];
                        } else
                            console.warn("property not found", arg);
                    }
                    jobController.configuration.data = jobData;
                }
            }

            Label {
                textFormat: Text.PlainText
                Layout.fillWidth: true
                Layout.fillHeight: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                visible: configLoader.status === Loader.Error
                wrapMode: Text.WordWrap
                text: configLoader.status === Loader.Error
                    ? _tr.i18n("Failed to load the configuration page for this action:\n\n%1", configLoader.sourceComponent.errorString())
                    : ""
            }

            // Not using a DialogButtonBox because it doesn't let us customize
            // the buttons and conditionally disable any of them, which we want
            RowLayout {
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.bottomMargin: Kirigami.Units.largeSpacing
                Layout.alignment: Qt.AlignRight

                Button {
                    text: _tr.i18n("Send")
                    icon.name: "document-send"
                    enabled: jobController.configuration
                        && jobController.configuration.isReady
                    onClicked: jobController.startJob()
                }

                Button {
                    text: _tr.i18n("Cancel")
                    icon.name: "dialog-cancel"
                    onClicked: jobController.cancel()
                }
            }
        }
    }
}
