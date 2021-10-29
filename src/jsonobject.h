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

    // BEGIN Inboud arguments

    /// Set urls, required for share and export plugins
    void setUrls(const QStringList &urls)
    {
        insert(QLatin1String("urls"), QJsonArray::fromStringList(urls));
    }
    QStringList urls()
    {
        return value(QLatin1String("urls")).toVariant().toStringList();
    }

    /// Set mime type, required for share plugins
    void setMimeType(const QString &mimeType)
    {
        insert(QLatin1String("mimeType"), mimeType);
    }
    QString mimeType()
    {
        return value(QLatin1String("mimeType")).toString();
    }

    /// Set the title, required for export plugins
    void setTitle(const QString &title)
    {
        insert(QLatin1String("title"), title);
    }
    QString title()
    {
        return value(QLatin1String("title")).toString();
    }
    // END

    // BEGIN Outbound arguments
    /**
     * Returns the url associated with the share result.
     * If the job has returned no url the returned string is empty
     */
    QString url()
    {
        return value(QLatin1String("url")).toString();
    }
    // END
};

}

#endif
