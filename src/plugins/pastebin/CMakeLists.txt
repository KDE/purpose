add_definitions(-DTRANSLATION_DOMAIN=\"purpose_pastebin\")
add_share_plugin(pastebinplugin pastebinplugin.cpp)
target_link_libraries(pastebinplugin KF5::KIOCore KF5::I18n)

