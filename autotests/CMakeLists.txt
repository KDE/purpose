set(bindest "${CMAKE_BINARY_DIR}/bin/qml")
set(qmldirdest "${bindest}/org/kde/purpose")

add_custom_command(
    OUTPUT ${qmldirdest}/qmldir
    COMMAND cmake -E make_directory -p ${qmldirdest}
    COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/src/quick/qmldir ${qmldirdest}
    DEPENDS ${CMAKE_SOURCE_DIR}/src/quick/qmldir
)

ecm_add_test(alternativesmodeltest.cpp ${qmldirdest}/qmldir
    TEST_NAME alternativesmodeltest
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test KF5::Purpose KF5::ConfigCore)
ecm_add_test(menutest.cpp ${qmldirdest}/qmldir
    TEST_NAME menutest
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test KF5::PurposeWidgets)

set_property(TEST menutest PROPERTY ENVIRONMENT KDE_PURPOSE_LOCAL_JOBS=1 APPEND)
set_property(TEST alternativesmodeltest PROPERTY ENVIRONMENT KDE_PURPOSE_LOCAL_JOBS=1 APPEND)
