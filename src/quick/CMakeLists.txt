add_library(purposequickplugin purposequickplugin.cpp)
target_link_libraries(purposequickplugin Qt5::Qml KF5Purpose)

install(TARGETS purposequickplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose)

install( FILES
        PurposeWizard.qml
        AlternativesView.qml
        DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose
)
