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
    public Q_SLOTS:
        void fileOpened(KIO::Job *, const QByteArray &);
        void uploadDone(KIO::Job *, const QByteArray &);
        void moreData(KIO::Job *, const QByteArray &);
        void uploadNeedData();
        void uploadFinal();
        void authenticated(bool);
        void loginDone(KIO::Job *job, const QByteArray &data);
    private:
        void setVideoInfo(QMap<QString, QString>& videoInfo);
        void checkWallet();
        KIO::TransferJob *openFileJob;
        KIO::TransferJob *uploadJob;
        QByteArray m_authToken;
        static const QByteArray developerKey;
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
};
#endif /* YOUTUBEJOB_H */
