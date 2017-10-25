add_definitions(-DTRANSLATION_DOMAIN=\"purpose_reviewboard\")
add_subdirectory(icons)

add_library(ReviewboardHelpers reviewboardjobs.cpp debug.cpp)
generate_export_header(ReviewboardHelpers EXPORT_FILE_NAME reviewboardhelpers_export.h)
target_link_libraries(ReviewboardHelpers KF5::CoreAddons KF5::I18n Qt5::Network)

add_share_plugin(reviewboardplugin reviewboardplugin.cpp)
target_link_libraries(reviewboardplugin Qt5::Widgets ReviewboardHelpers)

install(TARGETS ReviewboardHelpers ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

add_library(rbpurposequickplugin quick/reviewboardquickplugin.cpp quick/rbrepositoriesmodel.cpp quick/rbreviewslistmodel.cpp quick/reviewboardrc.cpp)
target_link_libraries(rbpurposequickplugin Qt5::Qml ReviewboardHelpers)

install(TARGETS rbpurposequickplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose/reviewboard)
install(FILES quick/qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose/reviewboard)
