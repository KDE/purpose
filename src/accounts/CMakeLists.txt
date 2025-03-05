ecm_add_qml_module(accountshelper URI org.kde.purpose.accountshelper GENERATE_PLUGIN_SOURCE DEPENDENCIES QtCore)

target_sources(accountshelper PRIVATE accountsmodel.cpp)

target_link_libraries(accountshelper PRIVATE Qt6::DBus Qt6::Gui)

ecm_finalize_qml_module(accountshelper)
