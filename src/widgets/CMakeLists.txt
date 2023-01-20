qt_add_resources(qmlfiles_SRCS purposewidgets-resources.qrc)
add_library(KF6PurposeWidgets menu.cpp ${qmlfiles_SRCS})
generate_export_header(KF6PurposeWidgets EXPORT_FILE_NAME purposewidgets_export.h BASE_NAME PurposeWidgets)
target_link_libraries(KF6PurposeWidgets
    PUBLIC
        KF6::Purpose
        Qt${QT_MAJOR_VERSION}::Widgets
    PRIVATE
        Qt${QT_MAJOR_VERSION}::Qml
        KF6::I18n
)
set_target_properties(KF6PurposeWidgets PROPERTIES VERSION ${PURPOSE_VERSION}
                                          SOVERSION ${PURPOSE_SOVERSION}
                                          EXPORT_NAME PurposeWidgets
)
target_include_directories(KF6PurposeWidgets PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
                                             "$<BUILD_INTERFACE:${Purpose_BINARY_DIR}>"
                                      INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/purposewidgets>"
)

install(TARGETS KF6PurposeWidgets EXPORT KF6PurposeTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})
add_library(KF6::PurposeWidgets ALIAS KF6PurposeWidgets)

ecm_generate_headers(PurposeWidgets_CamelCase_HEADERS
    HEADER_NAMES
        Menu
    REQUIRED_HEADERS PurposeWidgets_HEADERS
    PREFIX PurposeWidgets
)
install(FILES
        ${PurposeWidgets_CamelCase_HEADERS}
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/purposewidgets/PurposeWidgets/
        COMPONENT Devel)
install(FILES
  ${Purpose_BINARY_DIR}/src/widgets/purposewidgets_export.h
  ${PurposeWidgets_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/purposewidgets/purposewidgets COMPONENT Devel
)
