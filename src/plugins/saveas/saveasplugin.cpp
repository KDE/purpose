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

#include <KLocalizedString>
#include <QDebug>
#include <QTimer>
#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <KPluginFactory>
#include <KIO/CopyJob>

EXPORT_SHARE_VERSION

class SaveAsShareJob : public Purpose::Job
{
    Q_OBJECT
    public:
        SaveAsShareJob(QObject* parent) : Purpose::Job(parent) {
            setCapabilities(Killable | Suspendable);
        }

        virtual void start() override
        {
            QJsonArray inputUrls = data().value(QStringLiteral("urls")).toArray();

            if (inputUrls.isEmpty()) {
                setErrorText(i18n("No URLs to save"));
                setError(1);
                emitResult();
                return;
            }

            QList<QUrl> urls;
            foreach(const QJsonValue &val, inputUrls) {
                urls.append(QUrl(val.toString()));
            }

            const QUrl destination(data().value(QStringLiteral("destinationPath")).toString());
            job = KIO::copy(urls, destination);
            connect(job, &KJob::finished, this, &SaveAsShareJob::fileCopied);
        }

        bool doKill() override
        {
            return job->kill();
        }

        virtual QUrl configSourceCode() const override
        {
            QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("purpose/saveasplugin_config.qml"));
            Q_ASSERT(!path.isEmpty());
            return QUrl::fromLocalFile(path);
        }

        void fileCopied(KJob* job)
        {
            setError(job->error());
            setErrorText(job->errorText());
            if (job->error()==0) {
                Q_EMIT output({ { QStringLiteral("url"), qobject_cast<KIO::CopyJob*>(job)->destUrl().toString() } });
            }
            emitResult();
        }

    private:
        KJob* job;
};

class Q_DECL_EXPORT SaveAsPlugin : public Purpose::PluginBase
{
    Q_OBJECT
    public:
        SaveAsPlugin(QObject* p, const QVariantList& ) : Purpose::PluginBase(p) {}

        virtual Purpose::Job* share() const override
        {
            return new SaveAsShareJob(nullptr);
        }
};

K_PLUGIN_FACTORY_WITH_JSON(SaveAsShare, "saveasplugin.json", registerPlugin<SaveAsPlugin>();)

#include "saveasplugin.moc"
