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

#include <QDir>
#include <QTemporaryDir>
#include <QDebug>

DiffListModel::DiffListModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_initialDir(QDir::currentPath())
    , m_tempDir(0)
{
    refresh();
}

void DiffListModel::refresh()
{
    beginResetModel();
    m_values.clear();
    endResetModel();

    if (m_tempDir) {
        qCritical() << "DiffListModel::refresh() called while still active!";
        return;
    }

    // our CWD should be the directory from which the application was launched, which
    // may or may not be a git, mercurial or svn working copy, so we create a temporary
    // directory in which we initialise a git repository. This may be an empty repo.

    m_initialDir = QDir::currentPath();
    qWarning() << Q_FUNC_INFO << "initialDir=" << m_initialDir;
    m_tempDir = new QTemporaryDir;
    if (!m_tempDir->isValid()) {
        qCritical() << "DiffListModel::refresh() failed to create temporary directory"
            << m_tempDir->path() << ":" << m_tempDir->errorString();
    } else {
        if (QDir::setCurrent(m_tempDir->path())) {
            qWarning() << Q_FUNC_INFO << "tempCWD=" << m_tempDir->path() << "=" << QDir::currentPath();
            // the directory will be removed in receivedDiffRevs()
            m_tempDir->setAutoRemove(false);
            QProcess initGit;
            bool ok = false;
            // create the virgin git repo. This is a very cheap operation that should
            // never fail in a fresh temporary directory we ourselves created, so it
            // should be OK to do this with a synchronous call.
            initGit.start(QLatin1String("git init"));
            if (initGit.waitForStarted()) {
                ok = initGit.waitForFinished(500);
            }
            if (!ok) {
                qCritical() << "DiffListModel::refresh() : couldn't create temp. git repo:" << initGit.errorString();
            }
        } else {
            qCritical() << "DiffListModel::refresh() failed to chdir to" << m_tempDir->path();
        }
    }
    // create a list request with the current (= temp.) directory as the project directory.
    // This request is executed asynchronously, which is why we cannot restore the initial
    // working directory just yet, nor remove the temporary directory.
    Phabricator::DiffRevList* repo = new Phabricator::DiffRevList(QDir::currentPath(), this);
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

    // now we can restore the initial working directory and remove the temp directory
    // (in that order!).
    if (!QDir::setCurrent(m_initialDir)) {
        qCritical() << "DiffListModel::receivedDiffRevs() failed to restore initial directory" << m_initialDir;
    }
    if (m_tempDir) {
        m_tempDir->remove();
        delete m_tempDir;
        m_tempDir = 0;
    }
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
    if (m_status != status) {
        m_status = status;
        refresh();
    }
}
