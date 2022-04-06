ecm_add_qml_module(purposequickplugin URI "org.kde.purpose" VERSION 1.0)

target_sources(purposequickplugin PRIVATE purposequickplugin.cpp)

ecm_target_qml_sources(purposequickplugin SOURCES
    AlternativesView.qml
    JobView.qml
)

target_link_libraries(purposequickplugin
    Qt${QT_MAJOR_VERSION}::Qml
    KF5Purpose
)

ecm_finalize_qml_module(purposequickplugin DESTINATION ${KDE_INSTALL_QMLDIR})
