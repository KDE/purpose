find_program(ARCANIST arc)
if(NOT ARCANIST)
    message(WARNING "The phabricator plugin depends on having the 'arc' script available in the PATH")
else()
    message(STATUS "The 'arc' script was found as ${ARCANIST}")
endif()

add_definitions(-DTRANSLATION_DOMAIN=\"purpose_phabricator\")

add_subdirectory(icons)

set(PhabricatorHelper_SRCS phabricatorjobs.cpp)
ecm_qt_declare_logging_category(PhabricatorHelper_SRCS
    HEADER debug.h
    IDENTIFIER PLUGIN_PHABRICATOR
    CATEGORY_NAME kdevplatform.plugins.phabricator
    DEFAULT_SEVERITY Debug)

add_library(PhabricatorHelpers ${PhabricatorHelper_SRCS})
target_link_libraries(PhabricatorHelpers KF5::CoreAddons KF5::I18n)

add_executable(testphabricator tests/testphabricator.cpp)
ecm_mark_nongui_executable(testphabricator)
target_link_libraries(testphabricator PhabricatorHelpers Qt5::Core)

add_share_plugin(phabricatorplugin phabricatorplugin.cpp)
target_link_libraries(phabricatorplugin Qt5::Widgets PhabricatorHelpers)

install(TARGETS PhabricatorHelpers ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

add_library(phabricatorquickplugin quick/phabricatorquickplugin.cpp quick/difflistmodel.cpp quick/phabricatorrc.cpp)
target_link_libraries(phabricatorquickplugin Qt5::Qml PhabricatorHelpers)

install(TARGETS phabricatorquickplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose/phabricator)
install(FILES quick/qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/purpose/phabricator)
