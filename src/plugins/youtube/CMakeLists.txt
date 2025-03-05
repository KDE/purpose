add_definitions(-DTRANSLATION_DOMAIN=\"purpose6_youtube\")

add_share_plugin(youtubeplugin youtubeplugin.cpp youtubejob.cpp youtubejobcomposite.cpp)
target_link_libraries(youtubeplugin KF6::WidgetsAddons KF6::KIOCore Qt6::Network KF6::I18n KF6::Purpose)

