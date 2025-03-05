add_definitions(-DTRANSLATION_DOMAIN=\"purpose6_nextcloud\")

kaccounts_add_service(${CMAKE_CURRENT_SOURCE_DIR}/nextcloud-upload.service.in)

add_share_plugin(nextcloudplugin nextcloudplugin.cpp nextcloudjob.cpp)
target_link_libraries(nextcloudplugin KF6::KIOCore KF6::I18n KF6::Purpose KAccounts6)

ecm_add_qml_module(nextcloudhelper URI org.kde.purpose.nextcloud.private GENERATE_PLUGIN_SOURCE DEPENDENCIES QtCore)

target_sources(nextcloudhelper PRIVATE accountsmodel.cpp)

target_link_libraries(nextcloudhelper PRIVATE Qt6::DBus Qt6::Gui)

ecm_finalize_qml_module(nextcloudhelper)
