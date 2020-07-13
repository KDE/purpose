find_package(KF5KIO COMPONENTS)
find_package(KF5Notifications COMPONENTS)
if (NOT KF5KIO_FOUND OR NOT KF5Notifications_FOUND)
    return()
endif()


add_definitions(-DTRANSLATION_DOMAIN="purpose-fileitemaction")
include_directories(${CMAKE_SOURCE_DIR})

set(sharefileitemaction_debug_SRCS)
ecm_qt_declare_logging_category(sharefileitemaction_debug_SRCS
    HEADER purpose_fileitemaction_plugin_debug.h
    IDENTIFIER PURPOSE_FILEITEMACTION_PLUGIN_LOG
    CATEGORY_NAME kf.kio.widgets.fileitemactions.purpose
    OLD_CATEGORY_NAMES kf5.purpose.fileitemactionplugin
    DESCRIPTION "purpose fileitemaction plugin"
    EXPORT PURPOSE
)

kcoreaddons_add_plugin(sharefileitemaction
    SOURCES sharefileitemaction.cpp ${sharefileitemaction_debug_SRCS}
    JSON sharefileitemaction.json
    INSTALL_NAMESPACE "kf5/kfileitemaction")

target_link_libraries(sharefileitemaction
    KF5::KIOWidgets
    KF5::I18n
    KF5::Notifications
    KF5PurposeWidgets
)
