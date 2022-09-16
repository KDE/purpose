add_definitions(-DTRANSLATION_DOMAIN=\"purpose_reviewboard\")
add_subdirectory(icons)

ecm_qt_declare_logging_category(ReviewboardHelpers_SRCS
    HEADER debug.h
    IDENTIFIER PLUGIN_REVIEWBOARD
    CATEGORY_NAME kf.purpose.plugins.reviewboard
    OLD_CATEGORY_NAMES kdevplatform.plugins.reviewboard
    DESCRIPTION "purpose (reviewboard plugin)"
    EXPORT PURPOSE
)

add_library(ReviewboardHelpers reviewboardjobs.cpp ${ReviewboardHelpers_SRCS})
generate_export_header(ReviewboardHelpers EXPORT_FILE_NAME reviewboardhelpers_export.h)
target_link_libraries(ReviewboardHelpers KF5::CoreAddons KF5::I18n Qt${QT_MAJOR_VERSION}::Network)

add_share_plugin(reviewboardplugin reviewboardplugin.cpp)
target_link_libraries(reviewboardplugin Qt${QT_MAJOR_VERSION}::Widgets ReviewboardHelpers)

set_target_properties(ReviewboardHelpers PROPERTIES VERSION ${PURPOSE_VERSION} SOVERSION ${PURPOSE_SOVERSION})
install(TARGETS ReviewboardHelpers ${KF_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

add_library(rbpurposequickplugin quick/reviewboardquickplugin.cpp quick/rbrepositoriesmodel.cpp quick/rbreviewslistmodel.cpp quick/reviewboardrc.cpp)
target_link_libraries(rbpurposequickplugin Qt${QT_MAJOR_VERSION}::Qml ReviewboardHelpers)

install(TARGETS rbpurposequickplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/purpose/reviewboard)
install(FILES quick/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/purpose/reviewboard)
