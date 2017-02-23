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

#ifndef DIFFLISTMODEL_H
#define DIFFLISTMODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <QVector>

class KJob;
class QTemporaryDir;

class DiffListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status WRITE setStatus)
    public:
        DiffListModel(QObject* parent = Q_NULLPTR);

        void refresh();

        QVariant data(const QModelIndex &idx, int role) const Q_DECL_OVERRIDE;
        int rowCount(const QModelIndex & parent) const Q_DECL_OVERRIDE;

        QString status() const { return m_status; }

        void setStatus(const QString &status);

        void receivedDiffRevs(KJob* job);
        Q_SCRIPTABLE QVariant get(int row, const QByteArray &role);

    private:
        struct Value { QVariant summary; QVariant id; };
        QVector<Value> m_values;

        QString m_status;
        QString m_initialDir;
        QTemporaryDir *m_tempDir;
};

#endif
