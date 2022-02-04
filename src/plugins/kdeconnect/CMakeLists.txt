add_definitions(-DTRANSLATION_DOMAIN=\"purpose_kdeconnect\")
add_share_plugin(kdeconnectplugin kdeconnectplugin.cpp)
target_link_libraries(kdeconnectplugin KF5::I18n)
