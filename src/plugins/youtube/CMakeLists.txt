add_subdirectory(icons)
find_package(KAccounts REQUIRED)

add_share_plugin(youtubeplugin youtubemanager.cpp kipiplugin_youtube.cpp youtubejob.cpp youtubejobcomposite.cpp ../imgur/mpform.cpp)
target_link_libraries(youtubeplugin KF5::WidgetsAddons KF5::KIOCore Qt5::Network KF5::I18n KF5::Purpose KAccounts)

