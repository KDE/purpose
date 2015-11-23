/*
 Copyright 2015 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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

#include "menu.h"
#include <KI18n/KLocalizedContext>
#include <QQmlContext>
#include <purpose/configuration.h>
#include <purpose/alternativesmodel.h>
#include <QPointer>
#include <QDebug>
#include <QQmlApplicationEngine>

using namespace Purpose;

struct Purpose::MenuPrivate : public QObject
{
Q_OBJECT
public:
    ~MenuPrivate() { m_engine->deleteLater(); }
    MenuPrivate(Menu* q)
        : QObject(q)
        , m_engine(new QQmlApplicationEngine)
        , m_model(new AlternativesModel(q))
        , q(q)
    {
        m_engine->rootContext()->setContextObject(new KLocalizedContext(this));
        m_engine->load(QUrl(QStringLiteral("qrc:/JobDialog.qml")));
    }

    void trigger(int row) {
        Q_ASSERT(!m_engine->rootObjects().isEmpty());
        QObject* o = m_engine->rootObjects().first();

        o->setProperty("configuration", QVariant::fromValue<QObject*>(m_model->configureJob(row)));
        o->setProperty("q", QVariant::fromValue<QObject*>(q));
        o->setProperty("visible", true);
        o->setParent(q);
    }

public:
    QQmlApplicationEngine* m_engine;
    QPointer<AlternativesModel> m_model;
    Purpose::Menu* q;
};

Menu::Menu(QWidget* parent)
    : QMenu(parent)
    , d_ptr(new MenuPrivate(this))
{
    connect(d_ptr->m_model.data(), &AlternativesModel::inputDataChanged, this, &Menu::reload);
    connect(this, &QMenu::triggered, this, [this](QAction* action) {
        Q_D(Menu);
        d->trigger(action->property("row").toInt());
    });
}

void Menu::reload()
{
    Q_D(Menu);
    clear();
    for(int i=0, c=d->m_model->rowCount(); i != c; ++i) {
        QModelIndex idx = d->m_model->index(i);
        QAction* a = addAction(idx.data(Qt::DisplayRole).toString());
        a->setToolTip(idx.data(Qt::ToolTipRole).toString());
        a->setIcon(idx.data(Qt::DecorationRole).value<QIcon>());
        a->setProperty("pluginId", idx.data(AlternativesModel::PluginIdRole));
        a->setProperty("row", i);
    }

    setEnabled(!isEmpty());
}

AlternativesModel* Menu::model() const
{
    Q_D(const Menu);
    return d->m_model.data();
}

#include "menu.moc"
