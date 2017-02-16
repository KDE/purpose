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

#ifndef PHABRICATORRC_H
#define PHABRICATORRC_H

#include <QObject>
#include <QUrl>
#include <QJsonObject>

class PhabricatorRC : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl path READ path WRITE setPath)
    public:
        PhabricatorRC(QObject* parent = Q_NULLPTR);

        void setPath(const QUrl &path);

        QUrl path() const { return m_path; }

    Q_SIGNALS:
        void dataChanged();

    private:

        QUrl m_path;
};

#endif
