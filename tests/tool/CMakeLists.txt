qt_add_resources(sharetool_SRCS sharetool-resources.qrc)

add_executable(sharetool main.cpp ${sharetool_SRCS})
target_link_libraries(sharetool KF6::Purpose KF6::I18n KF6::CoreAddons Qt${QT_MAJOR_VERSION}::Qml Qt${QT_MAJOR_VERSION}::Widgets)
