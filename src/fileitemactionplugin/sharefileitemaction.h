/*
 * Copyright (C) 2011 Alejandro Fiestas Olivares <afiestas@kde.org>
 * Copyright (C) 2014 Aleix Pol Gonzalez <aleixpol@kde.org>
 * Copyright (C) 2018 Nicolas Fella <nicolas.fella@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SHAREFILEITEMACTION_H
#define SHAREFILEITEMACTION_H

#include <KFileItemListProperties>
#include <KAbstractFileItemActionPlugin>
#include <QLoggingCategory>

class QAction;
class KFileItemListProperties;
class QWidget;

namespace Purpose
{
class Menu;
}

Q_DECLARE_LOGGING_CATEGORY(PURPOSE_FILEITEMACTION)
class ShareFileItemAction : public KAbstractFileItemActionPlugin
{
Q_OBJECT
public:
    ShareFileItemAction(QObject* parent, const QVariantList& args);
    QList< QAction* > actions(const KFileItemListProperties& fileItemInfos, QWidget* parentWidget) override;

private:
    Purpose::Menu* m_menu;
};

#endif // SHAREFILEITEMACTION_H
