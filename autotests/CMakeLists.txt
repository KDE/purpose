ecm_add_tests(alternativesmodeltest LINK_LIBRARIES Qt5::Test KF5::Purpose KF5::ConfigCore)
ecm_add_tests(menutest LINK_LIBRARIES Qt5::Test KF5::PurposeWidgets)

set(bindest "${CMAKE_BINARY_DIR}/bin/qml")
# prepend ${bindest} to the current QML2_IMPORT_PATH env var,
# using the correct path separator
if(WIN32)
    set(TEST_ENV_VARS "QML2_IMPORT_PATH=${bindest}$<SEMICOLON>$ENV{QML2_IMPORT_PATH}")
else()
    set(TEST_ENV_VARS "QML2_IMPORT_PATH=${bindest}:$ENV{QML2_IMPORT_PATH}")
endif()
set_property(TEST menutest PROPERTY ENVIRONMENT "${TEST_ENV_VARS}" APPEND)
set_property(TEST alternativesmodeltest PROPERTY ENVIRONMENT "${TEST_ENV_VARS}" APPEND)
set_property(TEST menutest PROPERTY ENVIRONMENT KDE_PURPOSE_LOCAL_JOBS=1 APPEND)
set_property(TEST alternativesmodeltest PROPERTY ENVIRONMENT KDE_PURPOSE_LOCAL_JOBS=1 APPEND)
