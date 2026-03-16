add_definitions(-DTRANSLATION_DOMAIN=\"purpose6_youtube\")

if (KAccounts6_FOUND)
    kaccounts_add_service(${CMAKE_CURRENT_SOURCE_DIR}/google-youtube.service.in)
endif()

add_share_plugin(youtubeplugin youtubeplugin.cpp youtubejob.cpp youtubejobcomposite.cpp)
target_link_libraries(youtubeplugin KF6::WidgetsAddons KF6::KIOCore Qt6::Network KF6::I18n KF6::Purpose)

if (KAccounts6_FOUND)
    target_compile_definitions(youtubeplugin PRIVATE -DHAVE_KACCOUNTS=1)
    target_link_libraries(youtubeplugin KAccounts6)
else()
    target_compile_definitions(youtubeplugin PRIVATE -DHAVE_KACCOUNTS=0)
endif()

ecm_qt_declare_logging_category(youtubeplugin
    HEADER youtube_debug.h
    IDENTIFIER PLUGIN_YOUTUBE
    CATEGORY_NAME kf.purpose.plugins.youtube
    DESCRIPTION "purpose (youtube plugin)"
    EXPORT PURPOSE
)
