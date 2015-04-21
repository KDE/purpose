add_subdirectory(icons)
find_package(KAccounts REQUIRED)

kaccounts_add_service(${CMAKE_CURRENT_SOURCE_DIR}/google-youtube.service.in)

add_share_plugin(youtubeplugin kipiplugin_youtube.cpp youtubejob.cpp youtubejobcomposite.cpp)
target_link_libraries(youtubeplugin KF5::WidgetsAddons KF5::KIOCore Qt5::Network KF5::I18n KF5::Purpose KAccounts)

