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

#include <purpose/pluginbase.h>
#include "mpform.h"
#include <QDebug>
#include <QTimer>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KJob>
#include <KIO/TransferJob>
#include <KIO/StoredTransferJob>

EXPORT_SHARE_VERSION

Q_GLOBAL_STATIC_WITH_ARGS(const QUrl, imageImgurUrl, (QLatin1String("https://api.imgur.com/3/image")))
Q_GLOBAL_STATIC_WITH_ARGS(const QUrl, albumImgurUrl, (QLatin1String("https://api.imgur.com/3/album")))

// key associated with aleixpol@kde.org
Q_GLOBAL_STATIC_WITH_ARGS(const QString, YOUR_CLIENT_ID, (QLatin1String("0bffa5b4ac8383c")))

class ImgurShareJob : public Purpose::Job
{
    Q_OBJECT
    public:
        ImgurShareJob(QObject* parent)
            : Purpose::Job(parent)
            , m_pendingJobs(0)
        {}

        virtual void start() override
        {
            m_pendingJobs = 0;
            const QJsonArray urls = data().value(QStringLiteral("urls")).toArray();
            if (urls.isEmpty()) {
                qWarning() << "no urls to share" << urls << data();
                emitResult();
                return;
            }

            if (urls.count()>1) {
                KIO::TransferJob *tJob = KIO::storedHttpPost("", *albumImgurUrl, KIO::HideProgressInfo);
                tJob->setMetaData(QMap<QString,QString>{
                    { QStringLiteral("customHTTPHeader"), QStringLiteral("Authorization: Client-ID ") + *YOUR_CLIENT_ID }
                });
                connect(tJob, &KJob::result, this, &ImgurShareJob::albumCreated);
            } else {
                startUploading();
            }
        }

        QJsonObject processResponse(KJob* job) {
            KIO::StoredTransferJob *sjob = qobject_cast<KIO::StoredTransferJob *>(job);
            QJsonParseError error;
            const QJsonObject resultMap = QJsonDocument::fromJson(sjob->data(), &error).object();
            if (sjob->isErrorPage()) {
                setError(3);
                setErrorText(i18n("Error page returned"));
            } else if (job->error()) {
                setError(job->error());
                setErrorText(job->errorText());
            } else if (error.error) {
                setError(1);
                setErrorText(error.errorString());
            } else if (!resultMap.value(QStringLiteral("success")).toBool()) {
                setError(2);
                const QJsonObject dataMap = resultMap[QStringLiteral("data")].toObject();
                setErrorText(dataMap[QStringLiteral("error")].toString());
            } else {
                return resultMap[QStringLiteral("data")].toObject();
            }
            emitResult();
            return {};
        }

        void albumCreated(KJob* job) {
            const QJsonObject dataMap = processResponse(job);
            if (!dataMap.isEmpty()) {
                m_albumId = dataMap[QStringLiteral("id")].toString();
                m_albumDeleteHash = dataMap[QStringLiteral("deletehash")].toString();
                startUploading();
            }
        }

        void startUploading()
        {
            Q_EMIT infoMessage(this, i18n("Uploading files to imgur..."));
            const QJsonArray urls = data().value(QStringLiteral("urls")).toArray();
            foreach(const QJsonValue &val, urls) {
                QString u = val.toString();
                KIO::StoredTransferJob* job = KIO::storedGet(QUrl(u));
                connect(job, &KJob::finished, this, &ImgurShareJob::fileFetched);
                m_pendingJobs++;
            }
        }

        void fileFetched(KJob* j)
        {
            if (j->error()) {
                setError(j->error());
                setErrorText(j->errorText());
                emitResult();

                qDebug() << "error:" << j->errorText() << j->errorString();

                return;
            }

            MPForm form;
            KIO::StoredTransferJob* job = qobject_cast<KIO::StoredTransferJob*>(j);
            form.addFile(QStringLiteral("image"), job->url(), job->data());
            form.addPair(QStringLiteral("album"), m_albumDeleteHash, {});
            form.finish();

            KIO::StoredTransferJob *tJob = KIO::storedHttpPost(form.formData(), *imageImgurUrl, KIO::HideProgressInfo);
            tJob->setMetaData(QMap<QString,QString>{
                { QStringLiteral("content-type"), QString::fromLocal8Bit(form.contentType()) },
                { QStringLiteral("customHTTPHeader"), QStringLiteral("Authorization: Client-ID ") + *YOUR_CLIENT_ID }
            });
            connect(tJob, &KJob::result, this, &ImgurShareJob::imageUploaded);
        }

        void imageUploaded(KJob* job) {
            const QJsonObject dataMap = processResponse(job);
            if (!dataMap.isEmpty()) {
                const QString url = dataMap[QStringLiteral("link")].toString();
                Q_EMIT infoMessage(this, url, QStringLiteral("<a href='%1'>%1</a>").arg(url));
                --m_pendingJobs;

                if (m_pendingJobs == 0) {
                    const QString finalUrl = m_albumId.isEmpty() ? url : QStringLiteral("https://imgur.com/a/") + m_albumId;
                    setOutput({ { QStringLiteral("url"), finalUrl } });
                    emitResult();
                }
            }
        }

    private:
        QString m_albumId;
        QString m_albumDeleteHash;
        int m_pendingJobs;
};

class Q_DECL_EXPORT ImgurPlugin : public Purpose::PluginBase
{
    Q_OBJECT
    public:
        ImgurPlugin(QObject* p, const QVariantList& ) : Purpose::PluginBase(p) {}

        virtual Purpose::Job* createJob() const override
        {
            return new ImgurShareJob(nullptr);
        }
};

K_PLUGIN_FACTORY_WITH_JSON(ImgurShare, "imgurplugin.json", registerPlugin<ImgurPlugin>();)

#include "imgurplugin.moc"
