/*************************************************************************************
 *  Copyright (C) 2008-2011 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2008-2011 by Alex Fiestas <afiestas@kde.org>                       *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#ifndef YOUTUBEJOB_H
#define YOUTUBEJOB_H

#include <KPasswordDialog>
#include <KJob>
#include <KIO/Job>
#include <QMap>
#include <QPointer>
#include <QString>
#include <kwallet.h>

class YoutubeJob : public KJob
{
    Q_OBJECT
    public:
        YoutubeJob(const QUrl& url, const QString& title, const QString& tags, const QString& description, QObject* parent = Q_NULLPTR);
        virtual void start();
        bool showDialog();

        void login();

        QUrl outputUrl() const { return m_outputUrl; }

    public Q_SLOTS:
        void fileOpened(KIO::Job *, const QByteArray &);
        void uploadDone(KJob*);
        void moreData(KIO::Job *, const QByteArray &);
        void uploadNeedData(KIO::Job* job);
        void uploadFinal(KIO::Job* job);
        void authenticated(bool);
        void loginDone(KJob* job);
    private:
        void setVideoInfo(QMap<QString, QString>& videoInfo);
        void checkWallet();
        QByteArray m_authToken;
        static const QByteArray developerKey;

        QPointer<KIO::StoredTransferJob> uploadJob;
        QPointer<KIO::TransferJob> openFileJob;

        QUrl m_url;
        QString m_title;
        QString m_tags;
        QString m_description;

        QList<QUrl> mSelectedUrls;
        KWallet::Wallet *m_wallet;
        QString videoTitle;
        QString videoDesc;
        QString videoTags;
        KPasswordDialog *dialog;
        QUrl m_outputUrl;
};
#endif /* YOUTUBEJOB_H */
