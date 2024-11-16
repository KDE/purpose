add_definitions(-DTRANSLATION_DOMAIN=\"purpose6_kdeconnect\")

add_share_plugin(kdeconnectplugin kdeconnectplugin.cpp)

qt_add_dbus_interface(dbus_SRCS org.kde.kdeconnect.daemon.xml DaemonDbusInterface)
qt_add_dbus_interface(dbus_SRCS org.kde.kdeconnect.device.xml DeviceDbusInterface)

ecm_add_qml_module(kdeconnectQml URI "org.kde.purpose.kdeconnect" SOURCES devicesmodel.cpp ${dbus_SRCS} DEPENDENCIES QtCore GENERATE_PLUGIN_SOURCE)
target_link_libraries(kdeconnectQml PRIVATE Qt6::Gui Qt6::DBus)
ecm_finalize_qml_module(kdeconnectQml)
