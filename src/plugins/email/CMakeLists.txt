add_definitions(-DTRANSLATION_DOMAIN=\"purpose_email\")
add_share_plugin(emailplugin emailplugin.cpp)
target_link_libraries(emailplugin KF6::I18n KF6::Service KF6::KIOGui)
