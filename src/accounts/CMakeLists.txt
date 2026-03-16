# SPDX-FileCopyrightText: 2026 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-License-Identifier: BSD-2-Clause

ecm_add_qml_module(accountshelper
    URI org.kde.purpose.accountshelper
    GENERATE_PLUGIN_SOURCE
    DEPENDENCIES QtCore
)

target_sources(accountshelper PRIVATE accountsmodel.cpp)

target_link_libraries(accountshelper PRIVATE Qt6::DBus Qt6::Gui KF6::KIOGui)

if (KAccounts6_FOUND)
    target_compile_definitions(accountshelper PRIVATE -DHAVE_KACCOUNTS=1)
    target_link_libraries(accountshelper PRIVATE KAccounts6)
else()
    target_compile_definitions(accountshelper PRIVATE -DHAVE_KACCOUNTS=0)
endif()

ecm_qt_declare_logging_category(accountshelper
    HEADER accounts_debug.h
    IDENTIFIER PURPOSE_ACCOUNTS_LOG
    CATEGORY_NAME kf.purpose.accounts
    DESCRIPTION "purpose accounts integration"
    EXPORT PURPOSE
)

ecm_finalize_qml_module(accountshelper)
