add_definitions(-DTRANSLATION_DOMAIN=\"purpose_saveas\")
add_share_plugin(saveasplugin saveasplugin.cpp)
target_link_libraries(saveasplugin KF5::KIOCore KF5::I18n)
