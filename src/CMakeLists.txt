qt5_add_resources(purpose_SRCS standard-plugin-types.qrc)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake-paths.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/cmake-paths.h)

add_library(KF5Purpose
    pluginbase.cpp
    job.cpp
    alternativesmodel.cpp
    configuration.cpp
    helper.cpp
    externalprocess/processjob.cpp
    ${purpose_SRCS}
)

target_include_directories(KF5Purpose PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
                                             "$<BUILD_INTERFACE:${Purpose_BINARY_DIR}>"
                                      INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/purpose>"
)
target_link_libraries(KF5Purpose
    PUBLIC
        KF5::CoreAddons
        Qt5::Gui
    PRIVATE
        Qt5::DBus
        Qt5::Network #QLocalSocket
        KF5::ConfigCore
)
set_target_properties(KF5Purpose PROPERTIES VERSION ${PURPOSE_VERSION_STRING}
                                          SOVERSION ${PURPOSE_SOVERSION}
                                          EXPORT_NAME Purpose
)

install(TARGETS KF5Purpose EXPORT KF5PurposeTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
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
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/purpose/Purpose/
        COMPONENT Devel)
install(FILES
  ${Purpose_BINARY_DIR}/purpose/purpose_export.h
  ${Purpose_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/purpose/purpose COMPONENT Devel
)

add_executable(purposeprocess externalprocess/purposeprocess_main.cpp helper.cpp)
ecm_mark_nongui_executable(purposeprocess)
target_link_libraries(purposeprocess KF5::Purpose Qt5::Network Qt5::Widgets)
install(TARGETS purposeprocess DESTINATION ${KDE_INSTALL_LIBEXECDIR_KF5})

add_subdirectory(plugins)
add_subdirectory(quick)
add_subdirectory(widgets)
add_subdirectory(fileitemactionplugin)
