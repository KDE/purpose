cmake_minimum_required(VERSION 2.8.12)

project(Purpose)

find_package(ECM 5.48.0 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

set(REQUIRED_QT_VERSION 5.8.0)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Qml Gui DBus Widgets Network Test)
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

include(FeatureSummary)
include(GenerateExportHeader)

include(ECMSetupVersion)
include(ECMAddTests)
include(ECMInstallIcons)
include(ECMGenerateHeaders)
include(ECMMarkNonGuiExecutable)
include(ECMQtDeclareLoggingCategory)

set(KF5_VERSION "5.48.0") # handled by release scripts
set(KF5_DEP_VERSION "5.48.0") # handled by release scripts

find_package(KF5 ${KF5_DEP_VERSION} REQUIRED COMPONENTS CoreAddons I18n Config)

# Debian is a special snow flake and uses nodejs as binary name
# https://lists.debian.org/debian-devel-announce/2012/07/msg00002.html
if(EXISTS "/etc/debian_version") # is debian system?
    set(NODEJS_BINARY "nodejs" CACHE PATH "The binary name for the nodejs interpreter")
else() # sane system
    set(NODEJS_BINARY "node" CACHE PATH "The binary name for the nodejs interpreter")
endif()

ecm_setup_version(${KF5_VERSION} VARIABLE_PREFIX PURPOSE
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/purpose_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5PurposeConfigVersion.cmake"
                        SOVERSION 5)

add_subdirectory( src )
if (BUILD_TESTING)
    add_subdirectory( autotests )
    add_subdirectory( tests )
endif()

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
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/purpose COMPONENT Devel
)

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)

install( FILES purpose.categories DESTINATION ${KDE_INSTALL_CONFDIR} )

#TODO: Remove somewhen in the future
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/KDEExperimentalPurposeConfig.cmake"
  DESTINATION "${KDE_INSTALL_CMAKEPACKAGEDIR}/KDEExperimentalPurpose"
  COMPONENT Devel
)
