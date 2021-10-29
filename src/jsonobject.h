/*
    SPDX-FileCopyrightText: 2021 Alexander Lohnau <alexander.lohnau@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef PURPOSEJSONOBJECT_H
#define PURPOSEJSONOBJECT_H

#include <QJsonArray>
#include <QJsonObject>

namespace Purpose
{
/**
 * Utility class to set json values for Purpose plugins
 * @author Alexander Lohnau
 * @since 5.88
 */
class JsonObject : public QJsonObject
{
public:
    using QJsonObject::QJsonObject;
    using QJsonObject::operator=;

    /// Set urls, required for share and export plugins
    void setUrls(const QStringList &urls)
    {
        insert(QLatin1String("urls"), QJsonArray::fromStringList(urls));
    }

    /// Set mime type, required for share plugins
    void setMimeType(const QString &mimeType)
    {
        insert(QLatin1String("mimeType"), mimeType);
    }

    /// Set the title, required for export plugins
    void setTitle(const QString &title)
    {
        insert(QLatin1String("title"), title);
    }
};

}

#endif
