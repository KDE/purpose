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

#include "phabricatorrc.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

PhabricatorRC::PhabricatorRC(QObject* parent)
    : QObject(parent)
{}

void PhabricatorRC::setPath(const QUrl &filePath)
{
    if (filePath == m_path || !filePath.isLocalFile())
        return;

    //.arcconfig files are JSON files
    // TODO figure out the if/what/how of .arcconfig file contents
}

