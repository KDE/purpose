if (HAVE_DBUS)
    set(MAYBE_DBUS "Qt6::DBus")
endif ()

ecm_qt_declare_logging_category(external_process_debug_SRCS
    HEADER purpose_external_process_debug.h
    IDENTIFIER PURPOSE_EXTERNAL_PROCESS_LOG
    CATEGORY_NAME kf.purpose.externalprocess
    OLD_CATEGORY_NAMES kf5.purpose.externalprocess
    DESCRIPTION "purpose external process"
    EXPORT PURPOSE
)

ecm_add_test(alternativesmodeltest.cpp ../src/constrainthelpers.cpp ${external_process_debug_SRCS}
    TEST_NAME alternativesmodeltest
    LINK_LIBRARIES Qt6::Test ${MAYBE_DBUS} KF6::Purpose KF6::ConfigCore)

if (HAVE_DBUS)
    target_compile_definitions(alternativesmodeltest PRIVATE -DHAVE_QTDBUS=1)
else()
    target_compile_definitions(alternativesmodeltest PRIVATE -DHAVE_QTDBUS=0)
endif()

ecm_add_test(menutest.cpp
    TEST_NAME menutest
    LINK_LIBRARIES Qt6::Test KF6::PurposeWidgets)

set_property(TEST menutest PROPERTY ENVIRONMENT KDE_PURPOSE_LOCAL_JOBS=1 APPEND)
set_property(TEST alternativesmodeltest PROPERTY ENVIRONMENT KDE_PURPOSE_LOCAL_JOBS=1 APPEND)
