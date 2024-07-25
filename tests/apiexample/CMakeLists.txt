add_executable(qmlapiexample qmlapiexample.cpp)
ecm_add_qml_module(qmlapiexample URI "org.kde.purpose.apiexample" VERSION 1.0 GENERATE_PLUGIN_SOURCE)
ecm_target_qml_sources(qmlapiexample SOURCES ApiExample.qml)
target_link_libraries(qmlapiexample PRIVATE KF6::Purpose KF6::I18nQml KF6::CoreAddons Qt6::Qml Qt6::Widgets)