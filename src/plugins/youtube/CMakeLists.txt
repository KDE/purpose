add_subdirectory(icons)
find_package(KAccounts REQUIRED)

# TODO: remove when depending on KDE Applications 15.08
if (NOT DEFINED kaccounts_add_service)
    include(cmake/FindAccountsFileDir.cmake)

    function(kaccounts_add_service service_file_in)
        get_filename_component(service_filename ${service_file_in} NAME_WE)
        set(service_file ${CMAKE_CURRENT_BINARY_DIR}/${service_filename}.service)
        execute_process(COMMAND intltool-merge -x -u --no-translations ${service_file_in} ${service_file})
        install(FILES ${service_file} DESTINATION ${ACCOUNTS_SERVICES_DIR})
    endfunction()
endif ()

kaccounts_add_service(${CMAKE_CURRENT_SOURCE_DIR}/google-youtube.service.in)

add_share_plugin(youtubeplugin kipiplugin_youtube.cpp youtubejob.cpp youtubejobcomposite.cpp)
target_link_libraries(youtubeplugin KF5::WidgetsAddons KF5::KIOCore Qt5::Network KF5::I18n KF5::Purpose KAccounts)

