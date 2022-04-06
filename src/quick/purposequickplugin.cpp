/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "purposequickplugin.h"
#include "alternativesmodel.h"
#include "configuration.h"
#include "job.h"
#include "jobcontroller.h"
#include <qqml.h>

void PurposeQuickPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<Purpose::AlternativesModel>(uri, 1, 0, "PurposeAlternativesModel");
    qmlRegisterType<Purpose::JobController>(uri, 1, 0, "PurposeJobController");
    qmlRegisterUncreatableType<Purpose::Job>(uri, 1, 0, "PurposeJob", QStringLiteral("You're not supposed to instantiate jobs"));
    qmlRegisterUncreatableType<Purpose::Configuration>(uri, 1, 0, "PurposeConfiguration", QStringLiteral("You're not supposed to instantiate configurations"));
}
