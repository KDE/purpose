qt5_add_resources(qmlfiles_SRCS purposewidgets-resources.qrc)
add_library(KF5PurposeWidgets menu.cpp ${qmlfiles_SRCS})
generate_export_header(KF5PurposeWidgets EXPORT_FILE_NAME purposewidgets_export.h BASE_NAME PurposeWidgets)
target_link_libraries(KF5PurposeWidgets
    PUBLIC
        KF5::Purpose
        Qt5::Widgets
    PRIVATE
        Qt5::Qml
        KF5::I18n
)
set_target_properties(KF5PurposeWidgets PROPERTIES VERSION ${PURPOSE_VERSION_STRING}
                                          SOVERSION ${PURPOSE_SOVERSION}
                                          EXPORT_NAME PurposeWidgets
)
target_include_directories(KF5PurposeWidgets PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
                                             "$<BUILD_INTERFACE:${Purpose_BINARY_DIR}>"
                                      INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/purposewidgets>"
)

install(TARGETS KF5PurposeWidgets EXPORT KDEExperimentalPurposeTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
add_library(KF5::PurposeWidgets ALIAS KF5PurposeWidgets)

ecm_generate_headers(PurposeWidgets_CamelCase_HEADERS
    HEADER_NAMES
        Menu
    REQUIRED_HEADERS PurposeWidgets_HEADERS
    PREFIX PurposeWidgets
)
install(FILES
        ${Purpose_BINARY_DIR}/src/purposewidgets/purposewidgets_export.h
        ${PurposeWidgets_CamelCase_HEADERS}
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/purposewidgets/PurposeWidgets/
        COMPONENT Devel)
install(FILES
  ${PurposeWidgets_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/purposewidgets/purposewidgets COMPONENT Devel
)
