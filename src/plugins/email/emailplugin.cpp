/*
 Copyright 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
 Author: Daniel Vrátil <daniel.vratil@kdab.com>

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

#include <KPluginFactory>
#include <KLocalizedString>

#include <QStandardPaths>
#include <QProcess>
#include <QDesktopServices>
#include <QUrlQuery>
#include <QDebug>

EXPORT_SHARE_VERSION

namespace {

class EmailJob : public Purpose::Job
{
    Q_OBJECT
public:
    explicit EmailJob(QObject *parent = nullptr)
        : Purpose::Job(parent)
    {}

    void start() override
    {
        // Use xdg-mime to figure out what is the user's default email client
        const auto xdgmime = QStandardPaths::findExecutable(QStringLiteral("xdg-mime"));
        if (xdgmime.isEmpty()) {
            // xdg-utils not available, let Qt figure what to do for us...
            launchMailto();
            return;
        }

        auto xdgmimeProc = new QProcess(this);
        xdgmimeProc->setProgram(xdgmime);
        xdgmimeProc->setArguments({ QStringLiteral("query"), QStringLiteral("default"),
                                    QStringLiteral("x-scheme-handler/mailto") });
        connect(xdgmimeProc, static_cast<void(QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),
                this, &EmailJob::xdgMimeFinished);
        xdgmimeProc->start();
    }

    void xdgMimeFinished(int code, QProcess::ExitStatus status)
    {
        if (code != 0 || status != QProcess::NormalExit) {
            // Something went wrong, fallback to QDesktopServices
            launchMailto();
            return;
        }

        const auto proc = qobject_cast<QProcess*>(sender());
        const auto mailService = proc->readAllStandardOutput();
        qDebug() << "Default mailto handler:" << mailService;
        // Thunderbird is a special snowflake and cannot handle attachments via
        // the mailto schema, so we need to handle it ourselves
        if (mailService.contains("thunderbird")) {
            launchThunderbird();
        } else {
            launchMailto();
        }
    }

    void launchMailto()
    {
        const auto urls = data().value(QStringLiteral("urls")).toArray();

        QUrlQuery query;
        for (const auto &att : urls) {
            QUrl url(att.toString());
            if (url.isLocalFile()) {
                query.addQueryItem(QStringLiteral("attachment"), att.toString());
            } else {
                query.addQueryItem(QStringLiteral("body"), att.toString());
                query.addQueryItem(QStringLiteral("subject"), data().value(QStringLiteral("title")).toString());
            }
        }
        QUrl url;
        url.setScheme(QStringLiteral("mailto"));
        url.setQuery(query);
        if (!QDesktopServices::openUrl(url)) {
            setError(KJob::UserDefinedError);
            setErrorText(i18n("Failed to launch email client"));
        }
        emitResult();
    }

    void launchThunderbird()
    {
        // thunderbird -compose "attachment='file:///att1','file:///att2'"

        const auto tb = QStandardPaths::findExecutable(QStringLiteral("thunderbird"));
        if (tb.isEmpty()) {
            launchMailto();
            return;
        }

        const auto urls = data().value(QStringLiteral("urls")).toArray();
        QStringList attachments;
        QStringList args = QStringList{ QStringLiteral("-compose")};
        QString message;
        for (const auto &att : urls) {
            QUrl url(att.toString());
            if (url.isLocalFile()) {
                attachments.push_back(att.toString());
            } else {
                message += QStringLiteral("body='%1',subject='%2',").arg(url.toString()).arg(data().value(QStringLiteral("title")).toString());
            }
        }

        message +=(QStringLiteral("attachment='%1'").arg(attachments.join(QLatin1Char(','))));
        args.append(message);

        if (!QProcess::startDetached(tb, args)) {
            setError(KJob::UserDefinedError);
            setErrorText(i18n("Failed to launch email client"));
        }
        emitResult();
    }

};

}

class Q_DECL_EXPORT EmailPlugin : public Purpose::PluginBase
{
    Q_OBJECT
public:
    EmailPlugin(QObject *p, const QVariantList &)
        : Purpose::PluginBase(p)
    {}

    Purpose::Job *createJob() const override
    {
        return new EmailJob(nullptr);
    }
};

K_PLUGIN_CLASS_WITH_JSON(EmailPlugin, "emailplugin.json")

#include "emailplugin.moc"
