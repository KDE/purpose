/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import org.kde.purpose
import org.kde.kitemmodels as KItemModels

/*!
  \qmltype AlternativesView
  \inqmlmodule org.kde.purpose

  AlternativesView
*/
StackView {
    id: stack

    focus: true

    implicitHeight: currentItem.implicitHeight

    /*!
      \qmlproperty bool AlternativesView::running
     */
    property bool running: false

    /*!
      \qmlproperty string AlternativesView::pluginType
     */
    property alias pluginType: altsModel.pluginType

    /*!
      \qmlproperty var AlternativesView::inputData
     */
    property alias inputData: altsModel.inputData

    /*!
     */
    property Component highlight

    /*!
     */
    property Component header

    /*!
     */
    property Component footer

    /*!
      \qmlproperty enumeration AlternativesView::verticalLayoutDirection
     */
    property var verticalLayoutDirection: ListView.TopToBottom

    /*!
     */
    property Component delegate: Component {
        RowLayout {
            id: listDelegate

            required property string display
            required property int index

            width: ListView.view.width
            Label {
                Layout.fillWidth: true
                text: listDelegate.display
                elide: Text.ElideRight
            }
            Button {
                text: i18nd("libpurpose6_quick", "Use")
                onClicked: createJob(listDelegate.index);
            }
            Keys.onReturnPressed: createJob(listDelegate.index)
            Keys.onEnterPressed: createJob(listDelegate.index)
        }
    }

    /*!
      Signals when the job finishes, reports the
      \a error code and a text \a message.

      \a output will specify the output offered by the job
     */
    signal finished(var output, int error, string message)

    PurposeAlternativesModel {
        id: altsModel
    }

    /*!
      Adopts the job at the \a listViewIndex.
     */
    function createJob(listViewIndex : int) {
        const unsortedModelIndex = sortedModel.mapToSource(sortedModel.index(listViewIndex, 0)).row
        stack.push(jobComponent, {index: unsortedModelIndex})
    }

    /*!
      Clears and returns back to the initial view.
     */
    function reset() {
        for(; stack.depth>1; stack.pop())
        {}
    }

    initialItem: ListView {
        ScrollBar.vertical: ScrollBar {}
        focus: true
        model: KItemModels.KSortFilterProxyModel {
            id: sortedModel
            sortRoleName: "display"
            sourceModel: altsModel
        }

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
}
