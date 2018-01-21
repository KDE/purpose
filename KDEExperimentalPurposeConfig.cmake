find_package(KF5Purpose REQUIRED)
message(WARNING "Look up the Purpose framework using KF5Purpose")

set(BT ${CMAKE_BUILD_TYPE})
string(TOUPPER ${BT} BT)

set(props INTERFACE_INCLUDE_DIRECTORIES INTERFACE_LINK_LIBRARIES IMPORTED_LINK_DEPENDENT_LIBRARIES_${BT} IMPORTED_LOCATION_${BT} IMPORTED_SONAME_${BT})

function(_copyTarget A B)
    add_library(${B} SHARED IMPORTED)
    foreach(prop ${props})
        get_target_property(value ${A} ${prop})
        message(STATUS "set_target_properties(${B} PROPERTIES ${prop} ${value})")
        set_target_properties(${B} PROPERTIES ${prop} "${value}")
    endforeach()
endfunction()

_copyTarget(KF5::Purpose KDEExperimental::Purpose)
_copyTarget(KF5::PurposeWidgets KDEExperimental::PurposeWidgets)
