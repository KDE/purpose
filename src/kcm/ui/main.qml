/*
    SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.kirigami.delegates as KirigamiDelegates
import org.kde.kcmutils as KCM
import org.kde.purpose.kcm

KCM.ScrollViewKCM {
    id: root

    view: ListView {
        id: listview

        clip: true
        model: kcm.model

        delegate: QQC2.ItemDelegate {
            id: delegate

            required property int index
            required property string name
            required property string iconName
            required property string description
            required property int checkState

            text: name
            width: ListView.view.width

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                QQC2.CheckBox {
                    checkState: delegate.checkState

                    onToggled: kcm.model.setData(kcm.model.index(delegate.index, 0), checkState, Qt.CheckState)
                }

                Kirigami.IconTitleSubtitle {
                    icon.name: delegate.iconName

                    reserveSpaceForSubtitle: true

                    title: delegate.name
                    subtitle: delegate.description

                    Layout.fillWidth: true
                }
            }
        }
    }
}
