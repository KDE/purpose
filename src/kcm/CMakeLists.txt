# SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
# SPDX-License-Identifier: BSD-2-Clause

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_purpose\")

set(kcm_srcs
    kcmpurpose.cpp
    kcmpurpose.h
    pluginmodel.cpp
    pluginmodel.h
    ../helper.cpp
)
kcmutils_add_qml_kcm(kcm_purpose SOURCES ${kcm_srcs})
target_link_libraries(kcm_purpose PRIVATE
    KF6::Service
    KF6::KCMUtilsQuick
    Qt::DBus
    Qt::Gui
)
