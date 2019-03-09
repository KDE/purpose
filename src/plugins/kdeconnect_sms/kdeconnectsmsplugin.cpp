/*
 Copyright 2018 Nicolas Fella <nicolas.fella@gmx.de>

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

#include <purpose/pluginbase.h>
#include <QDebug>
#include <QProcess>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <KPluginFactory>
#include <KLocalizedString>

EXPORT_SHARE_VERSION

class KDEConnectSMSJob : public Purpose::Job
{
    Q_OBJECT
    public:
        KDEConnectSMSJob(QObject* parent)
            : Purpose::Job(parent)
        {}

        QStringList arrayToList(const QJsonArray& array)
        {
            QStringList ret;
            foreach(const QJsonValue& val, array) {
                ret += val.toString();
            }
            return ret;
        }

        void start() override
        {
            QJsonArray urlsJson = data().value(QStringLiteral("urls")).toArray();
            QString title = data().value(QStringLiteral("title")).toString();
            QString message = i18n("%1 - %2").arg(title).arg(arrayToList(urlsJson).join(QLatin1Char(' ')));

            QProcess::startDetached(QStringLiteral("kdeconnect-sms"), QStringList(QStringLiteral("--message")) << message);
            QTimer::singleShot(0, this, &KDEConnectSMSJob::emitResult);
        }
};

class Q_DECL_EXPORT KDEConnectSMSPlugin : public Purpose::PluginBase
{
    Q_OBJECT
    public:
        KDEConnectSMSPlugin(QObject* p, const QVariantList& ) : Purpose::PluginBase(p) {}

        Purpose::Job* createJob() const override
        {
            return new KDEConnectSMSJob(nullptr);
        }
};

K_PLUGIN_FACTORY_WITH_JSON(KDEConnectSMS, "kdeconnectsmsplugin.json", registerPlugin<KDEConnectSMSPlugin>();)

#include "kdeconnectsmsplugin.moc"
