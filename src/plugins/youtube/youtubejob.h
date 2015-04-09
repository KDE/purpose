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
#include <QMap>
#include <QPointer>
#include <QString>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include "../imgur/mpform.h"

class YoutubeJob : public KJob
{
    Q_OBJECT
    public:
        YoutubeJob(const QUrl& url, const QByteArray &token, const QString& title, const QStringList& tags, const QString& description, QObject* parent = Q_NULLPTR);
        void start() override;

        QString outputUrl() const { return m_output; }

    private:
        void fileFetched(KJob*);
        void createLocation();
        void locationCreated();
        void uploadVideo(const QByteArray& data);
        void videoUploaded();

        QUrl m_url;
        MPForm m_form;
        QByteArray m_token;
        QString m_output;
        QNetworkAccessManager m_manager;
        QByteArray m_metadata;
        QUrl m_uploadUrl;
};
#endif /* YOUTUBEJOB_H */
