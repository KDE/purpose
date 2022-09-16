qt_add_resources(purpose_SRCS standard-plugin-types.qrc)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake-paths.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/cmake-paths.h)

ecm_qt_declare_logging_category(external_process_debug_SRCS
    HEADER purpose_external_process_debug.h
    IDENTIFIER PURPOSE_EXTERNAL_PROCESS_LOG
    CATEGORY_NAME kf.purpose.externalprocess
    OLD_CATEGORY_NAMES kf5.purpose.externalprocess
    DESCRIPTION "purpose external process"
    EXPORT PURPOSE
)

add_library(KF5Purpose
    ${external_process_debug_SRCS}
    pluginbase.cpp
    job.cpp
    alternativesmodel.cpp
    configuration.cpp
    helper.cpp
    externalprocess/processjob.cpp
    jobcontroller.cpp
    ${purpose_SRCS}
)

target_include_directories(KF5Purpose PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
                                             "$<BUILD_INTERFACE:${Purpose_BINARY_DIR}>"
                                      INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/purpose>"
)
target_link_libraries(KF5Purpose
    PUBLIC
        KF5::CoreAddons
        Qt${QT_MAJOR_VERSION}::Gui
    PRIVATE
        Qt${QT_MAJOR_VERSION}::DBus
        Qt${QT_MAJOR_VERSION}::Network #QLocalSocket
        KF5::ConfigCore
)
set_target_properties(KF5Purpose PROPERTIES VERSION ${PURPOSE_VERSION}
                                          SOVERSION ${PURPOSE_SOVERSION}
                                          EXPORT_NAME Purpose
)

install(TARGETS KF5Purpose EXPORT KF5PurposeTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})
add_library(KF5::Purpose ALIAS KF5Purpose)

generate_export_header(KF5Purpose EXPORT_FILE_NAME ${Purpose_BINARY_DIR}/purpose/purpose_export.h BASE_NAME Purpose)
ecm_generate_headers(Purpose_CamelCase_HEADERS
    HEADER_NAMES
        PluginBase
        AlternativesModel
        Job
        Configuration
    REQUIRED_HEADERS Purpose_HEADERS
    PREFIX Purpose
)

install(FILES ${Purpose_CamelCase_HEADERS}
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/purpose/Purpose/
        COMPONENT Devel)
install(FILES
  ${Purpose_BINARY_DIR}/purpose/purpose_export.h
  ${Purpose_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/purpose/purpose COMPONENT Devel
)

add_executable(purposeprocess externalprocess/purposeprocess_main.cpp helper.cpp ${external_process_debug_SRCS})
ecm_mark_nongui_executable(purposeprocess)
target_link_libraries(purposeprocess KF5::Purpose Qt${QT_MAJOR_VERSION}::Network Qt${QT_MAJOR_VERSION}::Widgets)
install(TARGETS purposeprocess DESTINATION ${KDE_INSTALL_LIBEXECDIR_KF})

add_subdirectory(plugins)
add_subdirectory(quick)
add_subdirectory(widgets)
add_subdirectory(fileitemactionplugin)

ecm_qt_install_logging_categories(
    EXPORT PURPOSE
    FILE purpose.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
