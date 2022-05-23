add_definitions(-DTRANSLATION_DOMAIN=\"purpose_kdeconnectsms\")
add_share_plugin(kdeconnectsmsplugin kdeconnectsmsplugin.cpp)
target_link_libraries(kdeconnectsmsplugin KF5::I18n KF5::KIOGui)

