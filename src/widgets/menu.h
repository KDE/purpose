/*
 Copyright 2015 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PURPOSEMENU_H
#define PURPOSEMENU_H

#include <QMenu>
#include <purpose/purpose_export.h>

namespace Purpose
{
struct MenuPrivate;

class AlternativesModel;

class PURPOSE_EXPORT Menu : public QMenu
{
Q_OBJECT
public:
    Menu(QWidget* parent = Q_NULLPTR);

    AlternativesModel* model() const;
    void reload();

Q_SIGNALS:
    void finished(const QJsonObject &output, int error, const QString &message);

private:
    Q_DECLARE_PRIVATE(Menu)
    MenuPrivate *const d_ptr;
};

}

#endif
