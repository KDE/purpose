function(add_share_plugin name)
    kcoreaddons_add_plugin(${name} SOURCES ${ARGN} JSON "${name}.json" INSTALL_NAMESPACE "kf5/purpose")
    target_link_libraries(${name} Qt5::Core KF5::Purpose)
    set_target_properties(${name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/kf5/purpose")

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

include(ECMQMLModules)
ecm_find_qmlmodule(Ubuntu.OnlineAccounts 0.1)
ecm_find_qmlmodule(org.kde.kdeconnect 1.0)
ecm_find_qmlmodule(org.kde.kquickcontrolsaddons 2.0)

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
    add_subdirectory(nextcloud)
endif()
add_subdirectory(kdeconnect)
add_subdirectory(reviewboard)
add_subdirectory(phabricator)
add_subdirectory(email)
add_subdirectory(telegram)
add_subdirectory(bluetooth)
add_subdirectory(kdeconnect_sms)
