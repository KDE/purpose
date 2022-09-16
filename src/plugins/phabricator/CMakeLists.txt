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
    CATEGORY_NAME kf.purpose.plugins.phabricator
    OLD_CATEGORY_NAMES kdevplatform.plugins.phabricator
    DEFAULT_SEVERITY Debug
    DESCRIPTION "purpose (phabricator plugin)"
    EXPORT PURPOSE
)

add_library(PhabricatorHelpers ${PhabricatorHelper_SRCS})
generate_export_header(PhabricatorHelpers EXPORT_FILE_NAME phabricatorhelpers_export.h)
target_link_libraries(PhabricatorHelpers KF5::CoreAddons KF5::I18n)

add_executable(testphabricator tests/testphabricator.cpp)
ecm_mark_nongui_executable(testphabricator)
target_link_libraries(testphabricator PhabricatorHelpers Qt${QT_MAJOR_VERSION}::Core)

add_share_plugin(phabricatorplugin phabricatorplugin.cpp)
target_link_libraries(phabricatorplugin Qt${QT_MAJOR_VERSION}::Widgets PhabricatorHelpers)

set_target_properties(PhabricatorHelpers PROPERTIES VERSION ${PURPOSE_VERSION} SOVERSION ${PURPOSE_SOVERSION})
install(TARGETS PhabricatorHelpers ${KF_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

add_library(phabricatorquickplugin quick/phabricatorquickplugin.cpp quick/difflistmodel.cpp quick/phabricatorrc.cpp)
target_link_libraries(phabricatorquickplugin Qt${QT_MAJOR_VERSION}::Qml Qt${QT_MAJOR_VERSION}::Gui PhabricatorHelpers)

install(TARGETS phabricatorquickplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/purpose/phabricator)
install(FILES quick/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/purpose/phabricator)
