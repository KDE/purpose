add_definitions(-DTRANSLATION_DOMAIN=\"purpose6_nextcloud\")

add_share_plugin(nextcloudplugin nextcloudplugin.cpp nextcloudjob.cpp)
target_link_libraries(nextcloudplugin KF6::KIOCore KF6::I18n KF6::Purpose)
