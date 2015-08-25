#.rst:
# FindQMLModule
# -------
#
# Find QML modules with a single find_package() call.
#
#=============================================================================
# Copyright 2015 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

include(ECMFindModuleHelpersStub)

ecm_find_package_version_check(QMLModule)

if (NOT QMLModule_FIND_COMPONENTS)
    set(QMLModule_NOT_FOUND_MESSAGE "The QMLModule package requires at least one component")
    set(QMLModule_FOUND False)
    return()
endif()

set_package_properties(QMLModule PROPERTIES DESCRIPTION "QML Modules: checks failed or cannot be performed" TYPE RUNTIME)

include(FindPackageHandleStandardArgs)
include(FeatureSummary)

string (REPLACE ";" ", " THE_COMPONENTS "${QMLModule_FIND_COMPONENTS}")

set(QMLModule_VERSION "${QMLModule_FIND_VERSION}")
foreach(_module ${QMLModule_FIND_COMPONENTS})
    execute_process(COMMAND qmlplugindump "${_module}" "${QMLModule_FIND_VERSION}" OUTPUT_VARIABLE FUUUUU RESULT_VARIABLE ExitCode)

    if(ExitCode EQUAL 0)
        set(QMLModule_${_module}_FOUND TRUE)
        set(QMLModule_${_module}_VERSION "${QMLModule_FIND_VERSION}")
    else()
        set(QMLModule_${_module}_FOUND FALSE)
    endif()

    set_package_properties(QMLModule_${_module} PROPERTIES DESCRIPTION "QML Modules: ${THE_REQUIRED_COMPONENTS}" TYPE RUNTIME)
endforeach()

# Annoyingly, find_package_handle_standard_args requires you to provide
# REQUIRED_VARS even when using HANDLE_COMPONENTS, but all we actually
# care about is whether the required components were found. So we provide
# a dummy variable that is just set to something that will be printed
# on success.
set(_dummy_req_var "success")

find_package_handle_standard_args(QMLModule
    FOUND_VAR
        QMLModule_FOUND
    REQUIRED_VARS
        _dummy_req_var
    VERSION_VAR
        QMLModule_VERSION
    HANDLE_COMPONENTS
)

