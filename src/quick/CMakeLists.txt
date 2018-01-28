qt5_add_resources(purpose_SRCS qmlfiles.qrc)

add_library(purposequickplugin purposequickplugin.cpp ${purpose_SRCS})
target_link_libraries(purposequickplugin Qt5::Qml KF5Purpose)
set(bindest "${CMAKE_BINARY_DIR}/bin/qml/org/kde/purpose/")
set_target_properties(purposequickplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${bindest}")

add_custom_target(moveqmldir ALL
    COMMAND cmake -E make_directory -p ${bindest}
    COMMAND cmake -E copy ${CMAKE_CURRENT_SOURCE_DIR}/qmldir ${bindest}
)

install(TARGETS purposequickplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose)

# qmltypes
add_custom_target(qmltypes DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/plugins.qmltypes)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/plugins.qmltypes
    COMMAND qmlplugindump -nonrelocatable org.kde.purpose 1.0 ${KDE_INSTALL_QMLDIR} -output ${CMAKE_CURRENT_SOURCE_DIR}/plugins.qmltypes
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/plugins.qmltypes DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/purpose)
