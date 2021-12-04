/*
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "dropboxjob.h"
#include <KAccounts/Core>
#include <KAccounts/GetCredentialsJob>
#include <KIO/CopyJob>
#include <QDebug>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>

QList<QUrl> arrayToList(const QJsonArray &array)
{
    QList<QUrl> ret;
    for (const QJsonValue &val : array) {
        ret += val.toVariant().toUrl();
    }
    return ret;
}

void NextcloudJob::start()
{
    const Accounts::AccountId id = data().value(QStringLiteral("accountId")).toInt();
    auto credentialsJob = new GetCredentialsJob(id, this);

    connect(credentialsJob, &GetCredentialsJob::finished, this, &NextcloudJob::gotCredentials);

    credentialsJob->start();
}

void NextcloudJob::gotCredentials(KJob *job)
{
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    GetCredentialsJob *credentialsJob = qobject_cast<GetCredentialsJob *>(job);
    Q_ASSERT(credentialsJob);

    const QByteArray accessToken = credentialsJob->credentialsData()[QStringLiteral("AccessToken")].toByteArray();

    auto nam = new QNetworkAccessManager(this);

    const QList<QUrl> files = arrayToList(data().value(QStringLiteral("urls")).toArray());

    m_filesToTransfer = files.size();

    for (const QUrl &fileUrl : files) {
        // TODO check for local

        QFile file(fileUrl.toLocalFile());
        file.open(QIODevice::ReadOnly);
        const QByteArray fileContent = file.readAll();

        QNetworkRequest request(QUrl(QStringLiteral("https://content.dropboxapi.com/2/files/upload")));
        request.setRawHeader("Authorization", "Bearer " + accessToken);
        request.setRawHeader("Content-Type", "application/octet-stream");
        request.setRawHeader("Dropbox-API-Arg", "{\"path\":\"/" + QFileInfo(file).fileName().toUtf8() + "\"}");

        auto reply = nam->post(request, fileContent);

        connect(reply, &QNetworkReply::finished, this, [this, reply] {
            qDebug() << reply->error();
            qDebug() << reply->readAll();

            // TODO handle error, e.g. conflict

            --m_filesToTransfer;

            if (m_filesToTransfer == 0) {
                emitResult();
            }
        });
    }
}
