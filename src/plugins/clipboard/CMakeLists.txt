add_definitions(-DTRANSLATION_DOMAIN=\"purpose6_clipboard\")
add_share_plugin(clipboardplugin clipboardplugin.cpp)
target_link_libraries(clipboardplugin KF6::I18n Qt::Gui)

