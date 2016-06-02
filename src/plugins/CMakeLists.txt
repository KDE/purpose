function(add_share_plugin name)
    kcoreaddons_add_plugin(${name} SOURCES ${ARGN} JSON "${name}.json" INSTALL_NAMESPACE "purpose")
    target_link_libraries(${name} Qt5::Core KF5::Purpose)

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}_config.qml")
        install(FILES "${name}_config.qml" DESTINATION ${DATA_INSTALL_DIR}/purpose)
    endif()
endfunction()

set(KPACKAGE_RELATIVE_DATA_INSTALL_DIR "kpackage")
function(kpackage_install_package dir component root)
   set(install_dir ${ARGV3})
   if(NOT install_dir)
      set(install_dir ${KPACKAGE_RELATIVE_DATA_INSTALL_DIR})
   endif()
   install(DIRECTORY ${dir}/ USE_SOURCE_PERMISSIONS
           DESTINATION ${DATA_INSTALL_DIR}/${install_dir}/${root}/${component}
           PATTERN Messages.sh EXCLUDE)
endfunction()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(QMLModules)
find_qmlmodule(Ubuntu.OnlineAccounts 0.1)
find_qmlmodule(org.kde.kdeconnect 1.0)
find_qmlmodule(org.kde.kquickcontrolsaddons 2.0)

find_package(KAccounts)
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

    kpackage_install_package(twitter Twitter Purpose)
    kaccounts_add_service(${CMAKE_CURRENT_SOURCE_DIR}/twitter-microblog.service.in)
endif()
add_subdirectory(kdeconnect)
add_subdirectory(reviewboard)

install(FILES ExportPluginType.json DESTINATION ${DATA_INSTALL_DIR}/purpose/types)
