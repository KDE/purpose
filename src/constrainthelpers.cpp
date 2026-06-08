/*
 *  SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "constrainthelpers.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QMimeDatabase>
#include <QStandardPaths>

#if HAVE_QTDBUS
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#endif

#include "purpose_external_process_debug.h"

using namespace Qt::StringLiterals;

typedef bool (*matchFunction)(const QString &constraint, const QJsonValue &value);

static bool defaultMatch(const QString &constraint, const QJsonValue &value)
{
    return value == QJsonValue(constraint);
}

static bool mimeTypeMatch(const QString &constraint, const QJsonValue &value)
{
    if (value.isArray()) {
        const auto array = value.toArray();
        for (const QJsonValue &val : array) {
            if (mimeTypeMatch(constraint, val)) {
                return true;
            }
        }
        return false;
    } else if (value.isObject()) {
        for (const QJsonValue &val : value.toObject()) {
            if (mimeTypeMatch(constraint, val)) {
                return true;
            }
        }
        return false;
    } else if (constraint.contains(QLatin1Char('*'))) {
        const QRegularExpression re(QRegularExpression::wildcardToRegularExpression(constraint), QRegularExpression::CaseInsensitiveOption);
        return re.match(value.toString()).hasMatch();
    } else {
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForName(value.toString());
        return mime.inherits(constraint);
    }
}

static bool dbusMatch(const QString &constraint, const QJsonValue &value)
{
    Q_UNUSED(value)
#if HAVE_QTDBUS
    return QDBusConnection::sessionBus().interface()->isServiceRegistered(constraint);
#else
    Q_UNUSED(constraint)
    return false;
#endif
}

static bool executablePresent(const QString &constraint, const QJsonValue &value)
{
    Q_UNUSED(value)
    return !QStandardPaths::findExecutable(constraint).isEmpty();
}

static bool desktopFilePresent(const QString &constraint, const QJsonValue &value)
{
    Q_UNUSED(value)
    return !QStandardPaths::locate(QStandardPaths::ApplicationsLocation, constraint).isEmpty();
}

static QMap<QString, matchFunction> s_matchFunctions = {
    {QStringLiteral("mimeType"), mimeTypeMatch},
    {QStringLiteral("dbus"), dbusMatch},
    {QStringLiteral("application"), desktopFilePresent},
    {QStringLiteral("exec"), executablePresent},
};

const QRegularExpression constraintRx{QStringLiteral("(\\w+):(.*)")};

bool Purpose::constraintMatches(const KPluginMetaData &meta, const QJsonValue &constraint, const QJsonObject &inputData)
{
    // Treat an array as an OR
    if (constraint.isArray()) {
        const QJsonArray options = constraint.toArray();
        for (const auto &option : options) {
            if (constraintMatches(meta, option, inputData)) {
                return true;
            }
        }
        return false;
    }
    Q_ASSERT(constraintRx.isValid());
    QRegularExpressionMatch match = constraintRx.match(constraint.toString());
    if (!match.isValid() || !match.hasMatch()) {
        qCWarning(PURPOSE_EXTERNAL_PROCESS_LOG) << "wrong constraint" << constraint.toString();
        return false;
    }
    const QString propertyName = match.captured(1);
    const QString constrainedValue = match.captured(2);

    if (propertyName == u"mimeType"_s && !inputData.contains(propertyName)) {
        // The constraint doesn't pertain to this data type
        return true;
    }

    const bool acceptable = s_matchFunctions.value(propertyName, defaultMatch)(constrainedValue, inputData.value(propertyName));
    if (!acceptable) {
        // qCDebug(PURPOSE_EXTERNAL_PROCESS_LOG) << "not accepted" << meta.name() << propertyName << constrainedValue << *it;
    }
    return acceptable;
}
