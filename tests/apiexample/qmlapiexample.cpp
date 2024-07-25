/*
     SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>

     SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "qqml.h"

#include <KLocalizedQmlContext>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    KLocalization::setupLocalizedContext(&engine);
    engine.loadFromModule("org.kde.purpose.apiexample", "ApiExample");

    return app.exec();
}
