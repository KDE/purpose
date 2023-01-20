add_definitions(-DTRANSLATION_DOMAIN=\"purpose_youtube\")
add_subdirectory(icons)

kaccounts_add_service(${CMAKE_CURRENT_SOURCE_DIR}/google-youtube.service.in)

add_share_plugin(youtubeplugin youtubeplugin.cpp youtubejob.cpp youtubejobcomposite.cpp)
target_link_libraries(youtubeplugin KF6::WidgetsAddons KF6::KIOCore Qt${QT_MAJOR_VERSION}::Network KF6::I18n KF6::Purpose KAccounts)

