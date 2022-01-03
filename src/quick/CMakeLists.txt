qt_add_resources(purposequick_SRCS qmlfiles.qrc)

add_library(purposequickplugin purposequickplugin.cpp ${purposequick_SRCS})
target_link_libraries(purposequickplugin Qt${QT_MAJOR_VERSION}::Qml KF5Purpose)
set(bindest "${CMAKE_BINARY_DIR}/bin/qml/org/kde/purpose/")
set_target_properties(purposequickplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${bindest}")

add_custom_command(
    OUTPUT ${bindest}/qmldir
    COMMAND cmake -E make_directory -p ${bindest}
    COMMAND cmake -E copy ${CMAKE_CURRENT_SOURCE_DIR}/qmldir ${bindest}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/qmldir
)

install(TARGETS purposequickplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/purpose)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/purpose)

ecm_generate_qmltypes(org.kde.purpose 1.0 DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/purpose)
