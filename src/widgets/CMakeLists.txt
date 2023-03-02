set(PURPOSEWIDGETS_INSTALL_INCLUDEDIR "${KDE_INSTALL_INCLUDEDIR_KF}/PurposeWidgets")

qt_add_resources(qmlfiles_SRCS purposewidgets-resources.qrc)
add_library(KF6PurposeWidgets menu.cpp ${qmlfiles_SRCS})
generate_export_header(KF6PurposeWidgets EXPORT_FILE_NAME purposewidgets_export.h BASE_NAME PurposeWidgets)
target_link_libraries(KF6PurposeWidgets
    PUBLIC
        KF6::Purpose
        Qt6::Widgets
    PRIVATE
        Qt6::Qml
        KF6::I18n
)
set_target_properties(KF6PurposeWidgets PROPERTIES VERSION ${PURPOSE_VERSION}
                                          SOVERSION ${PURPOSE_SOVERSION}
                                          EXPORT_NAME PurposeWidgets
)
target_include_directories(KF6PurposeWidgets
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<BUILD_INTERFACE:${Purpose_BINARY_DIR}>"
    INTERFACE
        "$<INSTALL_INTERFACE:${PURPOSEWIDGETS_INSTALL_INCLUDEDIR}>"
)

install(TARGETS KF6PurposeWidgets EXPORT KF6PurposeTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})
add_library(KF6::PurposeWidgets ALIAS KF6PurposeWidgets)

ecm_generate_headers(PurposeWidgets_CamelCase_HEADERS
    HEADER_NAMES
        Menu
    REQUIRED_HEADERS PurposeWidgets_HEADERS
    PREFIX Purpose
)
install(
    FILES
        ${PurposeWidgets_CamelCase_HEADERS}
    DESTINATION ${PURPOSEWIDGETS_INSTALL_INCLUDEDIR}/Purpose # C++ namespace
    COMPONENT Devel
)
install(
    FILES
        ${Purpose_BINARY_DIR}/src/widgets/purposewidgets_export.h
        ${PurposeWidgets_HEADERS}
    DESTINATION ${PURPOSEWIDGETS_INSTALL_INCLUDEDIR}/purpose # C++ namespace
    COMPONENT Devel
)
