/*
 Copyright 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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

#include <QApplication>
#include <KAboutData>
#include <QCommandLineParser>
#include <QMimeDatabase>
#include <KLocalizedString>
#include <kdeclarative/kdeclarative.h>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include "qqml.h"
#include <purpose/alternativesmodel.h>
#include <purpose/job.h>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    KAboutData data(QStringLiteral("sharetool"), i18n("Share Tool"), QStringLiteral("1.0"), i18n("Share random information"), KAboutLicense::GPL);
    data.addAuthor(QStringLiteral("Aleix Pol i Gonzalez"), i18n("Implementation"), QStringLiteral("aleixpol@kde.org"));
    KAboutData::setApplicationData(data);

    QJsonObject inputData;
    QStringList files;
    {
        QCommandLineParser parser;
        parser.addPositionalArgument(QStringLiteral("files"), i18n("Files to share"), i18n("[files...]"));
        parser.addOption(QCommandLineOption(QStringLiteral("data"), i18n("Data tuple to initialize the process with"), QStringLiteral("json")));

        data.setupCommandLine(&parser);
        parser.addHelpOption();
        parser.addVersionOption();
        parser.process(app);
        data.processCommandLine(&parser);

        if (parser.isSet(QStringLiteral("data"))) {
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(parser.value(QStringLiteral("data")).toLatin1(), &error);
            if (error.error) {
                qCritical() << qPrintable(i18n("Error in the data argument formatting: %1 at %2", error.errorString(), error.offset));
                parser.showHelp(2);
            } else if (!doc.isObject()) {
                qCritical() << qPrintable(i18n("Error in the data argument type, it should be a json object."));
                parser.showHelp(3);
            }
            inputData = doc.object();
        }

        files = parser.positionalArguments();
        if (files.isEmpty() && !inputData.contains(QStringLiteral("urls"))) {
            qCritical() << qPrintable(i18n("Must specify some files to share"));
            parser.showHelp(1);
        }
    }

    QMimeType common;
    QJsonArray urls;
    if (!inputData.contains(QStringLiteral("urls")) || !inputData.contains(QStringLiteral("mimeType")))
    {
        QMimeDatabase db;
        for(const QString& file: files) {
            const QUrl url = QUrl::fromUserInput(file, QString(), QUrl::AssumeLocalFile);
            QMimeType type = db.mimeTypeForUrl(url);
            if (!common.isValid())
                common = type;
            else if(common.inherits(type.name())) {
                common = type;
            } else if(type.inherits(common.name())) {
                ;
            } else {
                common = db.mimeTypeForName(QStringLiteral("application/octet-stream"));
            }
            urls += url.toString();
        }
        inputData.insert(QStringLiteral("urls"), urls);
        inputData.insert(QStringLiteral("mimeType"), common.name());

    }

    QQmlApplicationEngine engine;
    KDeclarative::KDeclarative decl;
    decl.setDeclarativeEngine(&engine);
    decl.setupBindings();
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    engine.rootObjects().first()->setProperty("inputData", inputData);
    engine.rootObjects().first()->setProperty("visible", true);

    return app.exec();
}
