/*
    SPDX-FileCopyrightText: 2015 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "menu.h"
#include <KLocalizedContext>
#include <QDebug>
#include <QPointer>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <purpose/alternativesmodel.h>
#include <purpose/configuration.h>

using namespace Purpose;

class Purpose::MenuPrivate : public QObject
{
    Q_OBJECT
public:
    MenuPrivate(Menu *qq)
        : QObject(qq)
        , m_model(new AlternativesModel(qq))
        , q(qq)
    {
    }

    ~MenuPrivate() override
    {
        if (m_engine) {
            m_engine->deleteLater();
        }
    }

    void trigger(int row)
    {
        if (!m_engine) {
            m_engine = new QQmlApplicationEngine;
            m_engine->rootContext()->setContextObject(new KLocalizedContext(this));
            m_engine->load(QUrl(QStringLiteral("qrc:/JobDialog.qml")));
        }

        Q_ASSERT(!m_engine->rootObjects().isEmpty());
        QObject *o = m_engine->rootObjects().at(0);

        if (!o) {
            qWarning() << Q_FUNC_INFO << "object is NULL at m_engine" << m_engine << "rootObjects=" << m_engine->rootObjects();
            return;
        }

        o->setProperty("model", QVariant::fromValue(m_model.data()));
        o->setProperty("index", row);
        o->setProperty("visible", true);
        o->setProperty("q", QVariant::fromValue<QObject *>(q));
        o->setParent(q);

        QMetaObject::invokeMethod(o, "start");
    }

public:
    QQmlApplicationEngine *m_engine = nullptr;
    QPointer<AlternativesModel> m_model;
    Purpose::Menu *q;
};

Menu::Menu(QWidget *parent)
    : QMenu(parent)
    , d_ptr(new MenuPrivate(this))
{
    connect(d_ptr->m_model.data(), &AlternativesModel::inputDataChanged, this, &Menu::reload);
    connect(this, &QMenu::triggered, this, [this](QAction *action) {
        Q_D(Menu);
        d->trigger(action->property("row").toInt());
    });
}

void Menu::reload()
{
    Q_D(Menu);
    clear();
    for (int i = 0, c = d->m_model->rowCount(); i != c; ++i) {
        QModelIndex idx = d->m_model->index(i);
        QAction *a = addAction(idx.data(AlternativesModel::ActionDisplayRole).toString());
        a->setToolTip(idx.data(Qt::ToolTipRole).toString());
        a->setIcon(idx.data(Qt::DecorationRole).value<QIcon>());
        a->setProperty("pluginId", idx.data(AlternativesModel::PluginIdRole));
        a->setProperty("row", i);
    }

    setEnabled(!isEmpty());
}

AlternativesModel *Menu::model() const
{
    Q_D(const Menu);
    return d->m_model.data();
}

#include "menu.moc"
