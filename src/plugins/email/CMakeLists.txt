add_definitions(-DTRANSLATION_DOMAIN=\"purpose_email\")
add_share_plugin(emailplugin emailplugin.cpp)
target_link_libraries(emailplugin KF5::I18n KF5::Service KF5::KIOGui)
