function(add_share_plugin name)
    kcoreaddons_add_plugin(${name} SOURCES ${ARGN} JSON "${name}.json" INSTALL_NAMESPACE "purpose")
    target_link_libraries(${name} Qt5::Core KF5::Purpose)

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}_config.qml")
        install(FILES "${name}_config.qml" DESTINATION ${DATA_INSTALL_DIR}/purpose)
    endif()
endfunction()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(KAccounts REQUIRED)
find_package(QMLModule 0.1 COMPONENTS Ubuntu.OnlineAccounts)
find_package(QMLModule 1.0 COMPONENTS org.kde.kdeconnect)
find_package(KF5KIO ${KF5_DEP_VERSION} REQUIRED)

set_package_properties(KAccounts PROPERTIES
                       PURPOSE "Used to find your system-wide defined, for now only for YouTube."
                       TYPE OPTIONAL
)
set_package_properties(KF5KIO PROPERTIES TYPE REQUIRED)

add_subdirectory(saveas)
add_subdirectory(imgur)
add_subdirectory(pastebin)
add_subdirectory(ktp-sendfile)
if (KAccounts_FOUND)
    add_subdirectory(youtube)
endif()
add_subdirectory(kdeconnect)
add_subdirectory(reviewboard)

install(FILES ExportPluginType.json DESTINATION ${DATA_INSTALL_DIR}/purpose/types)
