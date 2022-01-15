/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import org.kde.purpose 1.0

StackView {
    id: stack
    focus: true

    implicitHeight: currentItem.implicitHeight

    property bool running: false
    /**
     * When this property is set to true, the job for  sharing the
     * specified file will be immediately executed.
     * When set to false, the signal clicked(index) will be emitted
     * instead and createJob(index) must be called manually afterwards.
     */
    property bool directMode: true
    property alias pluginType: altsModel.pluginType
    property alias inputData: altsModel.inputData
    property Component highlight
    property Component header
    property Component footer
    property variant verticalLayoutDirection: ListView.TopToBottom
    property Component delegate: Component {
        RowLayout {
            width: stack.width
            Label {
                Layout.fillWidth: true
                text: display
                elide: Text.ElideRight
            }
            Button {
                text: i18nd("libpurpose_quick", "Use")
                onClicked: _proceedNext(index);
            }
            Keys.onReturnPressed: _proceedNext(index)
            Keys.onEnterPressed: _proceedNext(index)
        }
    }

    /**
     * Signals when the job finishes, reports the
     * @p error code and a text @p message.
     *
     * @p output will specify the output offered by the job
     */
    signal finished(var output, int error, string message)

    /**
     * Signals when a button has been clicked when not in
     * direct mode (directMode = false)
     * @p index the job index that has been selected
     */
    signal clicked(var index)

    PurposeAlternativesModel {
        id: altsModel
    }

    /**
     * Adopts the job at the @p index.
     */
    function createJob(index) {
        //remove any busy indicator in case there is one
        if (stack.depth == 2) {
            stack.pop()
        }
        stack.push(jobComponent, {index: index})
    }

    /**
     * Clears and returns back to the initial view.
     */
    function reset() {
        for(; stack.depth>1; stack.pop())
        {}
    }

    function _proceedNext(index) {
        if (directMode) {
            createJob(index)
        } else {
            stack.push(waitingComponent)
            clicked(index)
        }
    }

    initialItem: ListView {
        ScrollBar.vertical: ScrollBar {}
        focus: true
        model: altsModel

        implicitHeight: contentHeight

        verticalLayoutDirection: stack.verticalLayoutDirection
        delegate: stack.delegate
        highlight: stack.highlight
        footer: stack.footer
        header: stack.header
    }

    Component {
        id: jobComponent

        JobView {
            id: jobView
            model: altsModel

            onStateChanged: {
                if (state === PurposeJobController.Finished || state === PurposeJobController.Error) {
                    stack.finished(jobView.job.output, jobView.job.error, jobView.job.errorString);
                } else if (state === PurposeJobController.Cancelled) {
                    stack.pop();
                }
            }

            Component.onCompleted: start()
        }
    }

    Component {
        id: waitingComponent

        Item {
            BusyIndicator {
                running: true
                anchors.centerIn: parent
            }
        }
    }
}
