add_definitions(-DTRANSLATION_DOMAIN=\"purpose_bluetoothplugin\")
add_share_plugin(bluetoothplugin bluetoothplugin.cpp)
target_link_libraries(bluetoothplugin KF5::I18n)
