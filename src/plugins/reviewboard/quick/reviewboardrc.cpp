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

#include "reviewboardrc.h"
#include <QRegExp>
#include <QFile>
#include <QHash>
#include <QTextStream>
#include <QDebug>

ReviewboardRC::ReviewboardRC(QObject* parent)
    : QObject(parent)
{}

void ReviewboardRC::setPath(const QUrl &filePath)
{
    if (filePath == m_path || !filePath.isLocalFile())
        return;

    //The .reviewboardrc files are python files, we'll read and if it doesn't work
    //Well bad luck. See: http://www.reviewboard.org/docs/rbtools/dev/rbt/configuration/

    QRegExp rx(QStringLiteral("([\\w_]+) *= *[\"'](.*)[\"']"));
    QFile f(filePath.toLocalFile());
    if(!f.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "couldn't open" << filePath;
        return;
    }

    QHash<QString, QString> values;
    QTextStream stream(&f);
    for(; !stream.atEnd(); ) {
        if(rx.exactMatch(stream.readLine())) {
            values.insert(rx.cap(1), rx.cap(2));
        }
    }

    if(values.contains(QStringLiteral("REVIEWBOARD_URL")))
        m_server = QUrl(values[QStringLiteral("REVIEWBOARD_URL")]);
    if(values.contains(QStringLiteral("REPOSITORY")))
        m_repository = values[QStringLiteral("REPOSITORY")];
    addExtraData(QStringLiteral("target_groups"), values[QStringLiteral("TARGET_GROUPS")]);
    addExtraData(QStringLiteral("target_people"), values[QStringLiteral("TARGET_PEOPLE")]);
    addExtraData(QStringLiteral("branch"), values[QStringLiteral("BRANCH")]);

    Q_EMIT dataChanged();
}

void ReviewboardRC::addExtraData(const QString& key, const QString &value)
{
    if (!value.isEmpty())
        m_extraData.insert(key, value);
}
