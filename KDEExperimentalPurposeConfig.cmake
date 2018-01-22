find_package(KF5Purpose REQUIRED)
message(WARNING "Look up the Purpose framework using KF5Purpose")

function(_copyTarget A B)
    add_library(${B} SHARED IMPORTED)

    set(props INTERFACE_INCLUDE_DIRECTORIES INTERFACE_LINK_LIBRARIES IMPORTED_CONFIGURATIONS)

    get_target_property(value ${A} IMPORTED_CONFIGURATIONS)
    foreach(cfg ${value})
        list(APPEND props IMPORTED_LINK_DEPENDENT_LIBRARIES_${cfg} IMPORTED_LOCATION_${cfg} IMPORTED_SONAME_${cfg})
    endforeach()

    foreach(prop ${props})
        get_target_property(value ${A} ${prop})
        if (NOT value)
            message(WARNING "missing property ${A} ${prop}")
        endif()
        set_target_properties(${B} PROPERTIES ${prop} "${value}")
    endforeach()
endfunction()

_copyTarget(KF5::Purpose KDEExperimental::Purpose)
_copyTarget(KF5::PurposeWidgets KDEExperimental::PurposeWidgets)
