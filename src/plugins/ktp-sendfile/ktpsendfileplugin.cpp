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

#include <purpose/pluginbase.h>
#include <QDebug>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <KPluginFactory>

EXPORT_SHARE_VERSION

class KTpSendFileShareJob : public Purpose::Job
{
    Q_OBJECT
    public:
        KTpSendFileShareJob(QObject* parent)
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

        virtual void start() override
        {
            QProcess* process = new QProcess(this);
            process->setProgram(QStringLiteral("ktp-send-file"));
            QJsonArray urlsJson = data().value(QStringLiteral("urls")).toArray();
            process->setArguments(arrayToList(urlsJson));
            connect(process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &KTpSendFileShareJob::jobFinished);

            process->start();
        }

        virtual QUrl configSourceCode() const override
        {
            return QUrl();
        }

        Q_SLOT void jobFinished(int code, QProcess::ExitStatus status)
        {
            setError(code);
            emitResult();
        }

    private:
};

class Q_DECL_EXPORT KTpSendFilePlugin : public Purpose::PluginBase
{
    Q_OBJECT
    public:
        KTpSendFilePlugin(QObject* p, const QVariantList& ) : Purpose::PluginBase(p) {}

        virtual Purpose::Job* share() const override
        {
            return new KTpSendFileShareJob(nullptr);
        }
};

K_PLUGIN_FACTORY_WITH_JSON(KTpSendFileShare, "ktpsendfileplugin.json", registerPlugin<KTpSendFilePlugin>();)

#include "ktpsendfileplugin.moc"
