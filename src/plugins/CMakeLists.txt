function(add_share_plugin name)
    kcoreaddons_add_plugin(${name} SOURCES ${ARGN} JSON "${name}.json" INSTALL_NAMESPACE "purpose")
    target_link_libraries(${name} Qt5::Core KF5::Purpose)

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}_config.qml")
        install(FILES "${name}_config.qml" DESTINATION ${DATA_INSTALL_DIR}/purpose)
    endif()
endfunction()

add_subdirectory(saveas)
add_subdirectory(imgur)
add_subdirectory(pastebin)
add_subdirectory(ktp-sendfile)
add_subdirectory(youtube)
add_subdirectory(kdeconnect)

install(FILES ExportPluginType.json DESTINATION ${DATA_INSTALL_DIR}/purpose/types)
