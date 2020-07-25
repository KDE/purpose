/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QStandardPaths>
#include <QUrl>
#include <QDebug>
#include <QMimeDatabase>

#include <purposewidgets/menu.h>
#include <purpose/alternativesmodel.h>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QScopedPointer<Purpose::Menu> menu(new Purpose::Menu);
    Purpose::AlternativesModel* model = menu->model();

    QJsonObject input;
    if (!app.arguments().isEmpty()) {
        QMimeDatabase mime;
        QUrl url = QUrl::fromUserInput(app.arguments().last());
        input = QJsonObject {
            { QStringLiteral("urls"), QJsonArray {url.toString()} },
            { QStringLiteral("mimeType"), mime.mimeTypeForUrl(url).name() }
        };
    } else {
        input = QJsonObject {
            { QStringLiteral("urls"), QJsonArray {QStringLiteral("http://kde.org")} },
            { QStringLiteral("mimeType"), QStringLiteral("dummy/thing") }
        };
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
