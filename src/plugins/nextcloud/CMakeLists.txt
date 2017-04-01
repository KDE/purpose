add_definitions(-DTRANSLATION_DOMAIN=\"purpose_nextcloud\")

kaccounts_add_service(${CMAKE_CURRENT_SOURCE_DIR}/nextcloud-upload.service.in)

add_share_plugin(nextcloudplugin nextcloudplugin.cpp nextcloudjob.cpp)
target_link_libraries(nextcloudplugin KF5::KIOCore KF5::I18n KF5::Purpose KAccounts)


