/*
    SPDX-FileCopyrightText: 2015 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
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

class Purpose::MenuPrivate : public QObject
{
Q_OBJECT
public:
    MenuPrivate(Menu* q)
        : QObject(q)
        , m_model(new AlternativesModel(q))
        , q(q)
    {

    }

    ~MenuPrivate() override {
        if (m_engine) {
            m_engine->deleteLater();
        }
    }

    void trigger(int row) {
        if (!m_engine) {
            m_engine = new QQmlApplicationEngine;
            m_engine->rootContext()->setContextObject(new KLocalizedContext(this));
            m_engine->load(QUrl(QStringLiteral("qrc:/JobDialog.qml")));
        }

        Q_ASSERT(!m_engine->rootObjects().isEmpty());
        QObject* o = m_engine->rootObjects().at(0);

        if (!o) {
            qWarning() << Q_FUNC_INFO << "object is NULL at m_engine" << m_engine << "rootObjects=" << m_engine->rootObjects();
            return;
        }
        auto config = m_model->configureJob(row);
        config->setUseSeparateProcess(false);
        o->setProperty("configuration", QVariant::fromValue<QObject*>(config));
        o->setProperty("q", QVariant::fromValue<QObject*>(q));
        o->setProperty("visible", true);
        o->setParent(q);
    }

public:
    QQmlApplicationEngine* m_engine = nullptr;
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
        QAction* a = addAction(idx.data(AlternativesModel::ActionDisplayRole).toString());
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
