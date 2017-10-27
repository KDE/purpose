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

#ifndef RBREPOSITORIESMODEL_H
#define RBREPOSITORIESMODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <KJob>

class RepositoriesModel : public QAbstractListModel
{
Q_OBJECT
Q_PROPERTY(QUrl server READ server WRITE setServer)
public:
    RepositoriesModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex & parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex & index, int role) const Q_DECL_OVERRIDE;

    void refresh();

    QUrl server() const { return m_server; }
    void setServer(const QUrl &server) {
        if (m_server != server) {
            m_server = server;
            refresh();
        }
    }

    void receivedProjects(KJob *j);
    Q_SCRIPTABLE int findRepository(const QString &name);

Q_SIGNALS:
    void repositoriesChanged();

private:
    struct Value {
        QVariant name;
        QVariant path;
        bool operator<(const Value &v1) const {
            return name.toString() < v1.name.toString();
        }
    };
    QVector<Value> m_values;
    QUrl m_server;
};

#endif
