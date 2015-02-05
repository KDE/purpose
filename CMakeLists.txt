cmake_minimum_required(VERSION 2.8.12)

project(Purpose)

find_package(ECM 1.7.0 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

set(REQUIRED_QT_VERSION "5.2")
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Qml Test)
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

include(FeatureSummary)
include(GenerateExportHeader)

include(ECMSetupVersion)
include(ECMAddTests)
include(ECMInstallIcons)
include(ECMGenerateHeaders)

set(KF5_VERSION "5.7.0") # handled by release scripts
set(KF5_DEP_VERSION "5.7.0") # handled by release scripts

find_package(KF5CoreAddons ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Config ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Declarative ${KF5_DEP_VERSION} REQUIRED) # for the sharetool

ecm_setup_version(${KF5_VERSION} VARIABLE_PREFIX PURPOSE
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/purpose_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5PurposeConfigVersion.cmake"
                        SOVERSION 5)

add_subdirectory( autotests )
add_subdirectory( src )

add_definitions(-DTRANSLATION_DOMAIN=\"purpose5\")
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
endif()

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5Purpose")

include(ECMPackageConfigHelpers)

ecm_configure_package_config_file(
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
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
)

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
