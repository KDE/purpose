add_definitions(-DTRANSLATION_DOMAIN=\"purpose_imgur\")
add_share_plugin(imgurplugin imgurplugin.cpp mpform.cpp)
target_link_libraries(imgurplugin KF5::KIOCore KF5::I18n KF5::Notifications)

