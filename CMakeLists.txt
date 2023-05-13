cmake_minimum_required(VERSION 3.16)

project(Purpose)

find_package(ECM 5.106.0 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

set(REQUIRED_QT_VERSION 5.15.2)
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(KDEGitCommitHooks)

include(FeatureSummary)

include(ECMGenerateExportHeader)
include(ECMSetupVersion)
include(ECMAddTests)
include(ECMInstallIcons)
include(ECMGenerateHeaders)
include(ECMMarkNonGuiExecutable)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMGenerateQmlTypes)
include(ECMQMLModules)
include(ECMQmlModule)
find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Qml Gui DBus Widgets Network Test)

set(KF_VERSION "5.107.0") # handled by release scripts
set(KF_DEP_VERSION "5.106.0") # handled by release scripts

find_package(KF5 ${KF_DEP_VERSION} REQUIRED COMPONENTS CoreAddons I18n Config Kirigami2 Notifications KIO)

find_package(KAccounts)

set_package_properties(KAccounts PROPERTIES
                       PURPOSE "Needed for YouTube and Nextcloud upload"
                       TYPE OPTIONAL
)

ecm_find_qmlmodule(Ubuntu.OnlineAccounts 0.1)
ecm_find_qmlmodule(org.kde.kquickcontrolsaddons 2.0)
ecm_find_qmlmodule(org.kde.prison 1.0)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

ecm_setup_version(${KF_VERSION} VARIABLE_PREFIX PURPOSE
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/purpose_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5PurposeConfigVersion.cmake"
                        SOVERSION 5)

ecm_set_disabled_deprecation_versions(
    # QT 5.15.2 Wait until libaccounts-qt has a release that doesn't use QString::null https://gitlab.com/accounts-sso/libaccounts-qt/issues/4
    KF 5.95
    KCOREADDONS 5.85 # K_EXPORT_PLUGIN_VERSION
)

add_subdirectory( src )
if (BUILD_TESTING)
    add_subdirectory( autotests )
    add_subdirectory( tests )
endif()

ki18n_install(po)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5Purpose")

include(CMakePackageConfigHelpers)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5PurposeConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5PurposeConfig.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/KF5PurposeConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5PurposeConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT KF5PurposeTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5PurposeTargets.cmake NAMESPACE KF5:: )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/purpose_version.h
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/purpose COMPONENT Devel
)

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)

#TODO: Remove somewhen in the future
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/KDEExperimentalPurposeConfig.cmake"
  DESTINATION "${KDE_INSTALL_CMAKEPACKAGEDIR}/KDEExperimentalPurpose"
  COMPONENT Devel
)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
