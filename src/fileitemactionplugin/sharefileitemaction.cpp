/*
    SPDX-FileCopyrightText: 2011 Alejandro Fiestas Olivares <afiestas@kde.org>
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: GPL-2.0-or-later
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
#include <KLocalizedString>
#include <KNotification>
#include <kio/global.h>

#include "menu.h"
#include "alternativesmodel.h"

K_PLUGIN_CLASS_WITH_JSON(ShareFileItemAction, "sharefileitemaction.json")

Q_LOGGING_CATEGORY(PURPOSE_FILEITEMACTION, "kf.kio.widgets.fileitemactions.purpose")

ShareFileItemAction::ShareFileItemAction(QObject* parent, const QVariantList& )
    : KAbstractFileItemActionPlugin(parent),
    m_menu(new Purpose::Menu())
{
    m_menu->setTitle(i18n("Share"));
    m_menu->setIcon(QIcon::fromTheme(QStringLiteral("document-share")));
    m_menu->model()->setPluginType(QStringLiteral("Export"));

    QObject::connect(m_menu, &Purpose::Menu::finished, [](const QJsonObject &output, int error, const QString &errorMessage) {
        if (error == 0 || error == KIO::ERR_USER_CANCELED) {
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
    QJsonArray urlsJson;

    for (const QUrl& url : fileItemInfos.urlList()) {
        urlsJson.append(url.toString());
    }

    m_menu->model()->setInputData(QJsonObject{
        { QStringLiteral("mimeType"), QJsonValue{!fileItemInfos.mimeType().isEmpty() ? fileItemInfos.mimeType() : QStringLiteral("*/*")} },
        { QStringLiteral("urls"), urlsJson }
    });
    m_menu->reload();
    m_menu->setParent(parentWidget, Qt::Popup);

    return {m_menu->menuAction()};
}

#include "sharefileitemaction.moc"
