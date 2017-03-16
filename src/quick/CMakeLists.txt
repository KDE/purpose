add_library(purposequickplugin purposequickplugin.cpp)
target_link_libraries(purposequickplugin Qt5::Qml KF5Purpose)

install(TARGETS purposequickplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose)

install( FILES
        PurposeWizard.qml
        AlternativesView.qml
        RunningJob.qml
        DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose
)

# qmltypes
add_custom_target(qmltypes DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/plugins.qmltypes)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/plugins.qmltypes
    COMMAND qmlplugindump -nonrelocatable org.kde.purpose 1.0 ${KDE_INSTALL_QMLDIR} -output ${CMAKE_CURRENT_SOURCE_DIR}/plugins.qmltypes
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/plugins.qmltypes DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/purpose)
