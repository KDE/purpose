// SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#ifndef EXPORTINTERFACE_H
#define EXPORTINTERFACE_H

#include <alternativesmodel.h>

namespace Purpose
{

/**
 * The @class ExportInterface allows you to easily export assets from applications
 *
 * It's a convenience implementation of the AlternativesModel that implements the
 * ExportPluginType.
 *
 * Note both urls and mimeType properties should be implemented so we can get our
 * alternatives.
 *
 * @since 5.90
 */
class PURPOSE_EXPORT ExportInterface : public AlternativesModel
{
    Q_OBJECT
    Q_PROPERTY(QStringList urls READ urls WRITE setUrls NOTIFY urlsChanged)
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType NOTIFY mimeTypeChanged)
public:
    ExportInterface(QObject *parent = nullptr);

    /**
     * sets @p urls to export
     **/
    void setUrls(const QStringList &urls);
    QStringList urls() const;

    /**
     * sets the @p mimetype we are exporting the urls as
     **/
    void setMimeType(const QString &mimetype);
    QString mimeType() const;

Q_SIGNALS:
    void urlsChanged();
    void mimeTypeChanged();
};

}

#endif // EXPORTINTERFACE_H
