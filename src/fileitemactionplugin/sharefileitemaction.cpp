/*
 * Copyright (C) 2011 Alejandro Fiestas Olivares <afiestas@kde.org>
 * Copyright (C) 2014 Aleix Pol Gonzalez <aleixpol@kde.org>
 * Copyright (C) 2018 Nicolas Fella <nicolas.fella@gmx.de>

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

#include "sharefileitemaction.h"

#include <QList>
#include <QAction>
#include <QWidget>
#include <QVariantList>
#include <QUrl>
#include <QIcon>
#include <QJsonArray>
#include <QDesktopServices>

#include <KPluginFactory>
#include <KPluginLoader>
#include <KLocalizedString>
#include <KNotification>

#include "menu.h"
#include "alternativesmodel.h"

K_PLUGIN_CLASS_WITH_JSON(ShareFileItemAction, "sharefileitemaction.json")

Q_LOGGING_CATEGORY(PURPOSE_FILEITEMACTION, "purpose.fileitemaction")

ShareFileItemAction::ShareFileItemAction(QObject* parent, const QVariantList& )
    : KAbstractFileItemActionPlugin(parent),
    m_menu(new Purpose::Menu())
{
    m_menu->setTitle(i18n("Share"));
    m_menu->setIcon(QIcon::fromTheme(QStringLiteral("document-share")));
    m_menu->model()->setPluginType(QStringLiteral("Export"));

    QObject::connect(m_menu, &Purpose::Menu::finished, [](const QJsonObject &output, int error, const QString &errorMessage) {
        if (error == 0) {
            if (output.contains(QLatin1String("url")))
                QDesktopServices::openUrl(QUrl(output.value(QLatin1String("url")).toString()));
        } else {
            KNotification::event(KNotification::Error, i18n("Error sharing"), errorMessage);
            qWarning() << "job failed with error" << error << errorMessage << output;
        }
    });
}

QList<QAction*> ShareFileItemAction::actions(const KFileItemListProperties& fileItemInfos, QWidget* parentWidget)
{
    Q_UNUSED(parentWidget);

    QJsonArray urlsJson;

    for (const QUrl& url : fileItemInfos.urlList()) {
        urlsJson.append(url.toString());
    }

    m_menu->model()->setInputData(QJsonObject{
        { QStringLiteral("mimeType"), QJsonValue{!fileItemInfos.mimeType().isEmpty() ? fileItemInfos.mimeType() : QStringLiteral("*/*")} },
        { QStringLiteral("urls"), urlsJson }
    });
    m_menu->reload();

    return {m_menu->menuAction()};
}

#include "sharefileitemaction.moc"
