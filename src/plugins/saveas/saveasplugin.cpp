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

#include <purpose/job.h>
#include <purpose/pluginbase.h>

#include <KLocalizedString>
#include <QDebug>
#include <QTimer>
#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileInfo>
#include <KPluginFactory>
#include <KIO/CopyJob>

EXPORT_SHARE_VERSION

static QUrl addPathToUrl(const QUrl &url, const QString &relPath)
{
    QString path = url.path();
    if (!path.endsWith(QLatin1Char('/'))) {
        path += QLatin1Char('/');
    }
    path += relPath;
    QUrl u(url);
    u.setPath(path);
    return u;
}

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
            bool containsData = false;
            foreach(const QJsonValue &val, inputUrls) {
                urls.append(QUrl(val.toString()));
                containsData |= urls.last().scheme() == QLatin1String("data");
            }

            m_dest = QUrl(data().value(QStringLiteral("destinationPath")).toString());
            if (containsData && !(urls.count() == 1 && m_dest.isLocalFile() && !QFileInfo(m_dest.toLocalFile()).isDir())) {
                for(const QUrl& url: urls) {
                    QUrl dest = addPathToUrl(m_dest, QStringLiteral("data"));
                    auto job = KIO::copy(url, dest);
                    connect(job, &KJob::finished, this, &SaveAsShareJob::fileCopied);
                    m_jobs.insert(job);
                }
            } else {
                auto job = KIO::copy(urls, m_dest);
                connect(job, &KJob::finished, this, &SaveAsShareJob::fileCopied);
                m_jobs.insert(job);
            }
        }

        bool doKill() override
        {
            bool killed = true;
            for(KJob* job: m_jobs)
                killed &= job->kill();
            return killed;
        }

        void fileCopied(KJob* job)
        {
            auto r = m_jobs.remove(job);
            Q_ASSERT(r);

            setError(error() + job->error());
            setErrorText(errorText() + QLatin1Char(' ') + job->errorText());
            if (job->error()==0 && m_jobs.isEmpty()) {
                setOutput({ { QStringLiteral("url"), m_dest.toString() } });
            }
            emitResult();
        }

    private:
        QUrl m_dest;
        QSet<KJob*> m_jobs;
};

class Q_DECL_EXPORT SaveAsPlugin : public Purpose::PluginBase
{
    Q_OBJECT
    public:
        SaveAsPlugin(QObject* p, const QVariantList& ) : Purpose::PluginBase(p) {}

        virtual Purpose::Job* createJob() const override
        {
            return new SaveAsShareJob(nullptr);
        }
};

K_PLUGIN_FACTORY_WITH_JSON(SaveAsShare, "saveasplugin.json", registerPlugin<SaveAsPlugin>();)

#include "saveasplugin.moc"
