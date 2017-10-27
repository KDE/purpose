/*
 * Copyright 2015 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef REVIEWBOARDRC_H
#define REVIEWBOARDRC_H

#include <QObject>
#include <QUrl>
#include <QJsonObject>

class ReviewboardRC : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl path READ path WRITE setPath)
    Q_PROPERTY(QUrl server READ server NOTIFY dataChanged)
    Q_PROPERTY(QString repository READ repository NOTIFY dataChanged)
    Q_PROPERTY(QJsonObject extraData READ extraData NOTIFY dataChanged)
    public:
        ReviewboardRC(QObject* parent = nullptr);

        void setPath(const QUrl &path);

        QUrl path() const { return m_path; }
        QUrl server() const { return m_server; }
        QString repository() const { return m_repository; }
        QJsonObject extraData() const { return m_extraData; }

    Q_SIGNALS:
        void dataChanged();

    private:
        void addExtraData(const QString& key, const QString &value);

        QUrl m_path;
        QUrl m_server;
        QString m_repository;
        QJsonObject m_extraData;
};

#endif
