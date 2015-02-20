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

#include <purpose/job.h>
#include <purpose/pluginbase.h>

#include <QDebug>
#include <QTimer>
#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <KPluginFactory>

EXPORT_SHARE_VERSION

class DummyShareJob : public Purpose::Job
{
    Q_OBJECT
    public:
        DummyShareJob(QObject* parent) : Purpose::Job(parent) {}

        virtual void start() override
        {
            QString destinationPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QLatin1Char('/') + data().value(QStringLiteral("destinationPath")).toString();
            QFile f(destinationPath);
            bool b = f.open(QIODevice::WriteOnly);
            Q_ASSERT(b);
            f.write(QJsonDocument(data()).toJson());
            QTimer::singleShot(100, this, [this](){ setPercent(10); });
            QTimer::singleShot(300, this, [this](){ setPercent(30); });
            QTimer::singleShot(600, this, [this](){ setPercent(80); });
            QTimer::singleShot(950, this, [this, destinationPath](){ Q_EMIT output( {{ QStringLiteral("url"), QUrl::fromLocalFile(destinationPath).toString() }} ); });
            QTimer::singleShot(1000, this, [this](){ emitResult(); });
        }

        virtual QUrl configSourceCode() const override
        {
            QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("purpose/dummyplugin_config.qml"));
            Q_ASSERT(!path.isEmpty());
            return QUrl::fromLocalFile(path);
        }
};

class Q_DECL_EXPORT DummyPlugin : public Purpose::PluginBase
{
    Q_OBJECT
    public:
        DummyPlugin(QObject* p, const QVariantList& ) : Purpose::PluginBase(p) {}

        virtual Purpose::Job* share() const override
        {
            return new DummyShareJob(nullptr);
        }
};

K_PLUGIN_FACTORY_WITH_JSON(DummyShare, "dummyplugin.json", registerPlugin<DummyPlugin>();)

#include "dummyplugin.moc"
