/*
    SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB a KDAB Group company info@kdab.com
    SPDX-FileContributor: Daniel Vrátil <daniel.vratil@kdab.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <purpose/pluginbase.h>

#include <KApplicationTrader>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QDebug>
#include <QDesktopServices>
#include <QJsonArray>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>
#include <QUrlQuery>

EXPORT_SHARE_VERSION

namespace
{
class EmailJob : public Purpose::Job
{
    Q_OBJECT
public:
    explicit EmailJob(QObject *parent = nullptr)
        : Purpose::Job(parent)
    {
    }

    void start() override
    {
        const KService::Ptr mailClient = KApplicationTrader::preferredService(QStringLiteral("x-scheme-handler/mailto"));

        // Thunderbird cannot handle attachments via the mailto schema, so we need to handle it ourselves
        if (mailClient->desktopEntryName().contains(QStringLiteral("thunderbird"))) {
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
        QStringList args = QStringList{QStringLiteral("-compose")};
        QString message;
        for (const auto &att : urls) {
            QUrl url(att.toString());
            if (url.isLocalFile()) {
                attachments.push_back(att.toString());
            } else {
                message += QStringLiteral("body='%1',subject='%2',").arg(url.toString()).arg(data().value(QStringLiteral("title")).toString());
            }
        }

        message += (QStringLiteral("attachment='%1'").arg(attachments.join(QLatin1Char(','))));
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
    {
    }

    Purpose::Job *createJob() const override
    {
        return new EmailJob(nullptr);
    }
};

K_PLUGIN_CLASS_WITH_JSON(EmailPlugin, "emailplugin.json")

#include "emailplugin.moc"
