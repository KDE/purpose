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

#include "kipiplugin_youtube.h"
#include "youtubejobcomposite.h"
#include "youtubejob.h"

#include <QAction>
#include <QApplication>
#include <QDesktopServices>
#include <QIcon>

#include <KPluginFactory>
#include <KAboutData>
#include <QMimeType>
#include <QUrl>
#include <kpassworddialog.h>
#include <QDebug>

K_PLUGIN_FACTORY_WITH_JSON(YoutubeShare, "youtubeplugin.json", registerPlugin<YoutubePlugin>();)

EXPORT_SHARE_VERSION

YoutubePlugin::YoutubePlugin(QObject* parent, const QVariantList& args)
    : Purpose::PluginBase(parent)
{
    Q_UNUSED(args);
}

Purpose::Job* YoutubePlugin::share() const
{
    return new YoutubeJobComposite;
}

#include "kipiplugin_youtube.moc"
