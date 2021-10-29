/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QUrl>

#include <purpose/alternativesmodel.h>
#include <purpose/jsonobject.h>
#include <purposewidgets/menu.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QScopedPointer<Purpose::Menu> menu(new Purpose::Menu);
    Purpose::AlternativesModel *model = menu->model();

    Purpose::JsonObject input;
    if (!app.arguments().isEmpty()) {
        QMimeDatabase mime;
        QUrl url = QUrl::fromUserInput(app.arguments().last());
        input.setUrls({url.toString()});
        input.setMimeType(mime.mimeTypeForUrl(url).name());
    } else {
        input.setUrls({QStringLiteral("http://kde.org")});
        input.setMimeType(QStringLiteral("dummy/thing"));
    }
    qDebug() << "sharing..." << input;

    model->setInputData(input);
    model->setPluginType(QStringLiteral("Export"));
    menu->reload();
    menu->exec();

    QObject::connect(menu.data(), &Purpose::Menu::finished, menu.data(), [&app](const QJsonObject &output, int error, const QString &errorMessage) {
        if (error != 0) {
            qDebug() << "job failed with error" << errorMessage;
        }
        qDebug() << "output:" << output;
        app.quit();
    });

    return app.exec();
}
