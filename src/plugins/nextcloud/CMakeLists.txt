add_definitions(-DTRANSLATION_DOMAIN=\"purpose6_nextcloud\")

if (KAccounts6_FOUND)
    kaccounts_add_service(${CMAKE_CURRENT_SOURCE_DIR}/nextcloud-upload.service.in)
endif()

add_share_plugin(nextcloudplugin nextcloudplugin.cpp nextcloudjob.cpp)
target_link_libraries(nextcloudplugin KF6::KIOCore KF6::I18n KF6::Purpose)

if (KAccounts6_FOUND)
    target_compile_definitions(nextcloudplugin PRIVATE -DHAVE_KACCOUNTS=1)
    target_link_libraries(nextcloudplugin KAccounts6)
else()
    target_compile_definitions(nextcloudplugin PRIVATE -DHAVE_KACCOUNTS=0)
endif()
