qt_add_resources(sharetool_SRCS sharetool-resources.qrc)

add_executable(sharetool main.cpp ${sharetool_SRCS})
target_link_libraries(sharetool KF5::Purpose KF5::I18n KF5::CoreAddons Qt${QT_MAJOR_VERSION}::Qml Qt${QT_MAJOR_VERSION}::Widgets)
