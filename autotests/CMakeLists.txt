ecm_add_tests(alternativesmodeltest LINK_LIBRARIES Qt5::Test KF5::Purpose KF5::ConfigCore)
ecm_add_tests(menutest LINK_LIBRARIES Qt5::Test KF5::PurposeWidgets)

set(bindest "${CMAKE_BINARY_DIR}/bin/qml")
set_property(TEST menutest PROPERTY ENVIRONMENT QML2_IMPORT_PATH=${bindest} APPEND)
set_property(TEST alternativesmodeltest PROPERTY ENVIRONMENT QML2_IMPORT_PATH=${bindest} APPEND)
set_property(TEST menutest PROPERTY ENVIRONMENT KDE_PURPOSE_LOCAL_JOBS=1 APPEND)
set_property(TEST alternativesmodeltest PROPERTY ENVIRONMENT KDE_PURPOSE_LOCAL_JOBS=1 APPEND)
