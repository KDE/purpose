/************************************************************************************
 * Copyright (C) 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>                *
 *                                                                                  *
 * This program is free software; you can redistribute it and/or                    *
 * modify it under the terms of the GNU General Public License                      *
 * as published by the Free Software Foundation; either version 2                   *
 * of the License, or (at your option) any later version.                           *
 *                                                                                  *
 * This program is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 * GNU General Public License for more details.                                     *
 *                                                                                  *
 * You should have received a copy of the GNU General Public License                *
 * along with this program; if not, write to the Free Software                      *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 ************************************************************************************/

#include <qtest.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QSignalSpy>

#include "menutest.h"
#include <purposewidgets/menu.h>
#include <purpose/alternativesmodel.h>

QTEST_MAIN(MenuTest)

QAction* saveAsAction(Purpose::Menu* menu)
{
    foreach(QAction* action, menu->actions()) {
        if (action->property("pluginId") == QLatin1String("saveasplugin")) {
            return action;
        }
    }

    Q_ASSERT(!"Couldn't find the saveas plugin");
    return nullptr;
}

void MenuTest::runJobTest()
{
    Purpose::Menu* menu = new Purpose::Menu;
    Purpose::AlternativesModel* model = menu->model();

    const QString tempfile = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QStringLiteral("/purposetest");
    QFile::remove(tempfile);
    const QJsonObject input = QJsonObject {
        { QStringLiteral("urls"), QJsonArray {QStringLiteral("http://kde.org")} },
        { QStringLiteral("mimeType"), QStringLiteral("dummy/thing") },
        { QStringLiteral("destinationPath"), QUrl::fromLocalFile(tempfile).url() }
    };
    model->setInputData(input);
    model->setPluginType(QStringLiteral("Export"));
    menu->reload();

    int error = -1;
    QJsonObject output;
    connect(menu, &Purpose::Menu::finished, menu, [&error, &output](const QJsonObject &_output, int _error, const QString &errorMessage) {
        error = _error;
        output = _output;
        if (error != 0) {
            qDebug() << "job failed with error" << errorMessage;
        }
    });

    QAction* action = saveAsAction(menu);
    QSignalSpy s(menu, &Purpose::Menu::finished);
    action->trigger();
    QVERIFY(s.count() || s.wait());
    QCOMPARE(error, 0);
    QCOMPARE(output.count(), 1);
    QVERIFY(QFile::remove(tempfile));

    delete menu;
}
