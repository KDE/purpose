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

#include "rbrepositoriesmodel.h"
#include "reviewboardjobs.h"

RepositoriesModel::RepositoriesModel(QObject* parent)
    : QAbstractListModel(parent)
{
    refresh();
}

void RepositoriesModel::refresh()
{
    if (m_server.isEmpty()) {
        beginResetModel();
        m_values.clear();
        endResetModel();
        Q_EMIT repositoriesChanged();
        return;
    }
    ReviewBoard::ProjectsListRequest* repo = new ReviewBoard::ProjectsListRequest(m_server, this);
    connect(repo, &ReviewBoard::ProjectsListRequest::finished, this, &RepositoriesModel::receivedProjects);
    repo->start();
}

QVariant RepositoriesModel::data(const QModelIndex & idx, int role) const
{
    if (!idx.isValid() || idx.column() != 0 || idx.row()>=m_values.count())
        return QVariant();

    switch(role) {
        case Qt::DisplayRole:
            return m_values[idx.row()].name;
        case Qt::ToolTipRole:
            return m_values[idx.row()].path;
        default:
            return QVariant();
    }
}

int RepositoriesModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : m_values.count();
}

void RepositoriesModel::receivedProjects(KJob *job)
{
    if (job->error()) {
        qWarning() << "received error when fetching repositories:" << job->error() << job->errorString();

        beginResetModel();
        m_values.clear();
        endResetModel();
        Q_EMIT repositoriesChanged();
        return;
    }

    ReviewBoard::ProjectsListRequest* pl=dynamic_cast<ReviewBoard::ProjectsListRequest*>(job);

    beginResetModel();
    m_values.clear();
    foreach(const QVariant& repo, pl->repositories()) {
        const QVariantMap repoMap = repo.toMap();
        m_values += Value { repoMap[QStringLiteral("name")], repoMap[QStringLiteral("path")] };
    }
    std::sort(m_values.begin(), m_values.end());
    endResetModel();
    Q_EMIT repositoriesChanged();
}

int RepositoriesModel::findRepository(const QString &name)
{
    QModelIndexList idxs = match(index(0,0), Qt::ToolTipRole, name, 1, Qt::MatchExactly);
    if(idxs.isEmpty()) {
        idxs = match(index(0,0), Qt::DisplayRole, QUrl(name).fileName(), 1, Qt::MatchExactly);
    }
    if(!idxs.isEmpty())
        return idxs.first().row();
    else
        qWarning() << "couldn't find the repository" << name;

    return -1;
}
