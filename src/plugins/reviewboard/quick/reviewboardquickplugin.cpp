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

#include "reviewboardquickplugin.h"
#include "rbrepositoriesmodel.h"
#include "rbreviewslistmodel.h"
#include "reviewboardrc.h"
#include <qqml.h>

void RBPurposeQuickPlugin::registerTypes(const char* uri)
{
    qmlRegisterType<RepositoriesModel>(uri, 1, 0, "RepositoriesModel");
    qmlRegisterType<ReviewsListModel>(uri, 1, 0, "ReviewsListModel");
    qmlRegisterType<ReviewboardRC>(uri, 1, 0, "ReviewboardRC");
}
