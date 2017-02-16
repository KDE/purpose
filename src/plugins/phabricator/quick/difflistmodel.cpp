/*
 * Copyright 2017 René J.V. Bertin <rjvbertin@gmail.com>
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

#include "difflistmodel.h"
#include "phabricatorjobs.h"

#include <QDebug>

DiffListModel::DiffListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    refresh();
}

void DiffListModel::refresh()
{
    beginResetModel();
    m_values.clear();
    endResetModel();
    return;

    // TODO : figure out how to set the baseDir string
    const QString baseDir;
    Phabricator::DiffRevList* repo = new Phabricator::DiffRevList(baseDir, this);
    connect(repo, &Phabricator::DiffRevList::finished, this, &DiffListModel::receivedDiffRevs);
    repo->start();
}

void DiffListModel::receivedDiffRevs(KJob* job)
{
    if (job->error() != 0) {
        qWarning() << "error getting differential revision list" << job->errorString();
        beginResetModel();
        m_values.clear();
        endResetModel();
        return;
    }

    const auto revs = dynamic_cast<Phabricator::DiffRevList*>(job)->reviews();
    beginResetModel();
    m_values.clear();
    foreach (const auto review, revs) {
        m_values += Value { review.second, review.first };
    }
    endResetModel();
}

QVariant DiffListModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid() || idx.column() != 0 || idx.row() >= m_values.size()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole:
            return m_values[idx.row()].summary;
        case Qt::ToolTipRole:
            return m_values[idx.row()].id;
    }
    return QVariant();
}

int DiffListModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : m_values.count();
}

QVariant DiffListModel::get(int row, const QByteArray &role)
{
    return index(row, 0).data(roleNames().key(role));
}

void DiffListModel::setStatus(const QString &status)
{
    qWarning() << Q_FUNC_INFO << "new status" << status;
    if (m_status != status) {
        m_status = status;
        refresh();
    }
}
