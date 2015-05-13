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

#ifndef REVIEWSLISTMODEL_H
#define REVIEWSLISTMODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <QVector>

class KJob;

class ReviewsListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QUrl server READ server WRITE setServer)
    Q_PROPERTY(QString username READ username WRITE setUsername)
    Q_PROPERTY(QString status READ status WRITE setStatus)
    Q_PROPERTY(QString repository READ repository WRITE setRepository)
    public:
        ReviewsListModel(QObject* parent = Q_NULLPTR);

        void refresh();

        QVariant data(const QModelIndex &idx, int role) const Q_DECL_OVERRIDE;
        int rowCount(const QModelIndex & parent) const Q_DECL_OVERRIDE;

        QUrl server() const { return m_server; }
        QString username() const { return m_username; }
        QString status() const { return m_status; }
        QString repository() const { return m_repository; }

        void setServer(const QUrl &server) {
            if (m_server != server) {
                m_server = server;
                refresh();
            }
        }

        void setUsername(const QString &username) {
            if (m_username != username) {
                m_username = username;
                refresh();
            }
        }

        void setStatus(const QString &status) {
            if (m_status != status) {
                m_status = status;
                refresh();
            }
        }

        void setRepository(const QString &repository) {
            if (m_repository != repository) {
                m_repository = repository;
                refresh();
            }
        }

        void receivedReviews(KJob* job);
        Q_SCRIPTABLE QVariant get(int row, const QByteArray &role);

    private:
        struct Value { QVariant summary; QVariant id; };
        QVector<Value> m_values;

        QUrl m_server;
        QString m_username;
        QString m_status;
        QString m_repository;
};

#endif
