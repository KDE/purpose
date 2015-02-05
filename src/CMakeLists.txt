add_library(KF5Purpose
    pluginbase.cpp
    job.cpp
    alternativesmodel.cpp
)
target_include_directories(KF5Purpose PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
                                      INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/Purpose>"
)
target_link_libraries(KF5Purpose KF5::CoreAddons Qt5::Gui)
kcoreaddons_desktop_to_json(KF5Purpose plugins/ExportPluginType.desktop)
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
    REQUIRED_HEADERS Purpose_HEADERS
    PREFIX Purpose
)
install(FILES ${Purpose_CamelCase_HEADERS}
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/Purpose/Purpose
        COMPONENT Devel)
install(FILES
  ${Purpose_BINARY_DIR}/purpose/purpose_export.h
  ${Purpose_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/Purpose/purpose COMPONENT Devel
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ExportPluginType.json DESTINATION ${DATA_INSTALL_DIR}/purpose/types)

add_subdirectory(plugins)
add_subdirectory(tool)
add_subdirectory(quick)
