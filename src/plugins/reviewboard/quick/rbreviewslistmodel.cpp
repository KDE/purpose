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

#include "rbreviewslistmodel.h"
#include "reviewboardjobs.h"

ReviewsListModel::ReviewsListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    refresh();
}

void ReviewsListModel::refresh()
{
    if (m_server.isEmpty() || m_repository.isEmpty()) {
        beginResetModel();
        m_values.clear();
        endResetModel();
        return;
    }

    ReviewBoard::ReviewListRequest* repo = new ReviewBoard::ReviewListRequest(m_server, m_username, m_status, this);
    connect(repo, &ReviewBoard::ReviewListRequest::finished, this, &ReviewsListModel::receivedReviews);
    repo->start();
}

void ReviewsListModel::receivedReviews(KJob* job)
{
    if (job->error() != 0) {
        qWarning() << "error review list" << job->errorString();

        beginResetModel();
        m_values.clear();
        endResetModel();
        return;
    }

    QMultiHash<QString, Value> reviews;
    QVariantList revs = dynamic_cast<ReviewBoard::ReviewListRequest*>(job)->reviews();
    beginResetModel();
    m_values.clear();
    foreach(const QVariant& review, revs) {
        QVariantMap reviewMap = review.toMap();
        QVariantMap repoMap = reviewMap[QStringLiteral("links")].toMap()[QStringLiteral("repository")].toMap();
        if (repoMap[QStringLiteral("title")].toString() == m_repository) {
            m_values += Value { reviewMap[QStringLiteral("summary")], reviewMap[QStringLiteral("id")] };
        }
    }
    endResetModel();
}

QVariant ReviewsListModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid() || idx.column() != 0 || idx.row()>=m_values.size())
        return QVariant();

    switch(role) {
        case Qt::DisplayRole:
            return m_values[idx.row()].summary;
        case Qt::ToolTipRole:
            return m_values[idx.row()].id;
    }
    return QVariant();
}

int ReviewsListModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : m_values.count();
}

QVariant ReviewsListModel::get(int row, const QByteArray &role)
{
    return index(row, 0).data(roleNames().key(role));
}
