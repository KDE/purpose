find_program(ARCANIST arc)
if(NOT ARCANIST)
    message(WARNING "The phabricator plugin depends on having the 'arc' script available in the PATH")
else()
    message(STATUS "The 'arc' script was found as ${ARCANIST}")
endif()

add_definitions(-DTRANSLATION_DOMAIN=\"purpose_phabricator\")

add_subdirectory(icons)
# 
# add_library(PhabricatorHelpers phabricatorjobs.cpp debug.cpp)
# target_link_libraries(PhabricatorHelpers KF5::CoreAddons KF5::I18n Qt5::Network)
# 
# add_share_plugin(phabricatorplugin phabricatorplugin.cpp)
# target_link_libraries(phabricatorplugin Qt5::Widgets PhabricatorHelpers)
# 
# install(TARGETS PhabricatorHelpers ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
# 
# add_library(rbpurposequickplugin quick/phabricatorquickplugin.cpp quick/rbrepositoriesmodel.cpp quick/rbreviewslistmodel.cpp quick/phabricatorrc.cpp)
# target_link_libraries(rbpurposequickplugin Qt5::Qml PhabricatorHelpers)
# 
# install(TARGETS rbpurposequickplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose/phabricator)
# install(FILES quick/qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose/phabricator)
