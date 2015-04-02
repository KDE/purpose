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

#include "alternativesmodeltest.h"
#include <purpose/job.h>
#include <purpose/alternativesmodel.h>

QTEST_MAIN(AlternativesModelTest)

int saveAsRow(Purpose::AlternativesModel* model)
{
    for(int i=0, c=model->rowCount(); i<c; ++i) {
        QString pluginId = model->index(i).data(Purpose::AlternativesModel::PluginIdRole).toString();
        if (pluginId == QLatin1String("saveasplugin")) {
            return i;
        }
    }

    Q_ASSERT(!"Couldn't find the saveas plugin");
    return -1;
}

void AlternativesModelTest::runJobTest()
{
    Purpose::AlternativesModel model;

    const QString tempfile = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QStringLiteral("/purposetest");
    QJsonObject input = QJsonObject {
        {QStringLiteral("urls"), QJsonArray {QStringLiteral("http://kde.org")} },
        {QStringLiteral("mimeType"), QStringLiteral("dummy/thing") }
    };
    model.setInputData(input);

    model.setPluginType(QStringLiteral("Export"));
    Purpose::Job* job = model.createJob(saveAsRow(&model));
    QVERIFY(job);
    QVERIFY(!job->isReady());
    input.insert(QStringLiteral("destinationPath"), QUrl::fromLocalFile(tempfile).url()),
    job->setData(input);
    QVERIFY(job->isReady());
    job->start();

    QSignalSpy s(job, &KJob::finished);
    QVERIFY(s.wait());
    if (job->error()) {
        qWarning() << "error!" << job->error() << job->errorString() << job->errorText();
    }
    QCOMPARE(job->error(), 0);
    QVERIFY(QFile::remove(tempfile));
}
