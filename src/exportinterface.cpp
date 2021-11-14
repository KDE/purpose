// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2021 <copyright holder> <email>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "exportinterface.h"
#include <QJsonArray>

using namespace Purpose;

ExportInterface::ExportInterface(QObject* parent)
    : AlternativesModel(parent)
{
    setPluginType(QStringLiteral("Export"));
}

void ExportInterface::setUrls(const QStringList &urls)
{
    auto input = inputData();
    auto newUrls = QJsonArray::fromStringList(urls);
    QJsonValue val = input.value(QLatin1String("urls"));
    if (newUrls != val.toArray()) {
        input.insert(QLatin1String("urls"), newUrls);
        setInputData(input);
        Q_EMIT urlsChanged();
    }
}

QStringList ExportInterface::urls() const
{
    const auto urlsArray = inputData().value(QLatin1String("urls")).toArray();
    QStringList ret;
    ret.reserve(urlsArray.size());
    for (const QJsonValue &val : ret) {
        ret.append(val.toString());
    }
    return ret;
}

void ExportInterface::setMimeType(const QString &mimetype)
{
    auto input = inputData();
    QJsonValue val = input.value(QLatin1String("mimeType"));
    if (mimetype != val.toString()) {
        input.insert(QLatin1String("mimeType"), mimetype);
        setInputData(input);
        Q_EMIT mimeTypeChanged();
    }
}

QString ExportInterface::mimeType() const
{
    return inputData().value(QLatin1String("mimetype")).toString();
}
