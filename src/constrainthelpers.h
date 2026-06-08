/*
 *  SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <KPluginMetaData>

namespace Purpose
{
bool constraintMatches(const KPluginMetaData &meta, const QJsonValue &constraint, const QJsonObject &inputData);
};
