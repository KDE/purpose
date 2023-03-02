set(PURPOSEWIDGETS_INSTALL_INCLUDEDIR "${KDE_INSTALL_INCLUDEDIR_KF}/purposewidgets")

qt_add_resources(qmlfiles_SRCS purposewidgets-resources.qrc)
add_library(KF5PurposeWidgets menu.cpp ${qmlfiles_SRCS})

ecm_generate_export_header(KF5PurposeWidgets
    EXPORT_FILE_NAME purposewidgets_export.h
    BASE_NAME PurposeWidgets
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 5.104
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

target_link_libraries(KF5PurposeWidgets
    PUBLIC
        KF5::Purpose
        Qt${QT_MAJOR_VERSION}::Widgets
    PRIVATE
        Qt${QT_MAJOR_VERSION}::Qml
        KF5::I18n
)
set_target_properties(KF5PurposeWidgets PROPERTIES VERSION ${PURPOSE_VERSION}
                                          SOVERSION ${PURPOSE_SOVERSION}
                                          EXPORT_NAME PurposeWidgets
)
target_include_directories(KF5PurposeWidgets
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<BUILD_INTERFACE:${Purpose_BINARY_DIR}>"
    INTERFACE
        "$<INSTALL_INTERFACE:${PURPOSEWIDGETS_INSTALL_INCLUDEDIR}>"
)

install(TARGETS KF5PurposeWidgets EXPORT KF5PurposeTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})
add_library(KF5::PurposeWidgets ALIAS KF5PurposeWidgets)

ecm_generate_headers(PurposeWidgets_CamelCase_HEADERS
    HEADER_NAMES
        Menu
    REQUIRED_HEADERS PurposeWidgets_HEADERS
    PREFIX Purpose
)
install(
    FILES
        ${PurposeWidgets_CamelCase_HEADERS}
    DESTINATION ${PURPOSEWIDGETS_INSTALL_INCLUDEDIR}/Purpose
    COMPONENT Devel
)
install(
    FILES
        ${Purpose_BINARY_DIR}/src/widgets/purposewidgets_export.h
        ${PurposeWidgets_HEADERS}
    DESTINATION ${PURPOSEWIDGETS_INSTALL_INCLUDEDIR}/purpose
    COMPONENT Devel
)

# provide compat headers for old C++ namespace-part-of-filename includes
if(NOT EXCLUDE_DEPRECATED_BEFORE_AND_AT STREQUAL "CURRENT" AND
   EXCLUDE_DEPRECATED_BEFORE_AND_AT VERSION_LESS 5.104.0)
    install(
        FILES compatheaders/Menu
        DESTINATION ${PURPOSEWIDGETS_INSTALL_INCLUDEDIR}/PurposeWidgets
        COMPONENT Devel
    )
    install(
        FILES compatheaders/menu.h
        DESTINATION ${PURPOSEWIDGETS_INSTALL_INCLUDEDIR}/purposewidgets
        COMPONENT Devel
    )
endif()
