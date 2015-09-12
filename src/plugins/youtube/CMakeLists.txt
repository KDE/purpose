add_subdirectory(icons)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# TODO: remove when depending on KDE Applications 15.08
if (NOT DEFINED kaccounts_add_service)
    find_dependency(AccountsQt5 REQUIRED)
    include(cmake/FindAccountsFileDir.cmake)

    find_program(INTLTOOLMERGE intltool-merge)
    if(NOT INTLTOOLMERGE)
        message(FATAL_ERROR "couldn't find intltool-merge")
    endif()

    function(kaccounts_add_service service_file_in)
        get_filename_component(service_filename ${service_file_in} NAME_WE)
        set(service_file ${CMAKE_CURRENT_BINARY_DIR}/${service_filename}.service)


        execute_process(COMMAND ${INTLTOOLMERGE} -x -u --no-translations ${service_file_in} ${service_file})
        install(FILES ${service_file} DESTINATION ${ACCOUNTS_SERVICES_DIR})
    endfunction()
    include_directories(${ACCOUNTSQT_INCLUDE_DIRS})
endif ()

kaccounts_add_service(${CMAKE_CURRENT_SOURCE_DIR}/google-youtube.service.in)

add_share_plugin(youtubeplugin youtubeplugin.cpp youtubejob.cpp youtubejobcomposite.cpp)
target_link_libraries(youtubeplugin KF5::WidgetsAddons KF5::KIOCore Qt5::Network KF5::I18n KF5::Purpose KAccounts)

