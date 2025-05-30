/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef PURPOSEALTERNATIVESMODEL_H
#define PURPOSEALTERNATIVESMODEL_H

#include <QAbstractListModel>
#include <QJsonObject>

#include <purpose/purpose_export.h>

namespace Purpose
{
class Configuration;
class AlternativesModelPrivate;

/*!
 * \class Purpose::AlternativesModel
 * \inheaderfile Purpose/AlternativesModel
 * \inmodule Purpose
 *
 * \brief Interface for client applications to share data.
 *
 * Lists all the alternatives to share a specified type of data then provides
 * a method to trigger a job.
 */
class PURPOSE_EXPORT AlternativesModel : public QAbstractListModel
{
    Q_OBJECT
    /*!
     * \property Purpose::AlternativesModel::pluginType
     *
     * \brief Specifies the type of the plugin we want to list.
     *
     * \sa inputData
     */
    Q_PROPERTY(QString pluginType READ pluginType WRITE setPluginType NOTIFY pluginTypeChanged)

    /*!
     * \property Purpose::AlternativesModel::inputData
     *
     * \brief Specifies the information that will be given to the plugin once it's started.
     *
     * \note some plugins might be filtered out based on this setting
     */
    Q_PROPERTY(QJsonObject inputData READ inputData WRITE setInputData NOTIFY inputDataChanged)

    /*!
     * \property Purpose::AlternativesModel::disabledPlugins
     *
     * \brief Provides a list of plugin names to have filtered out.
     */
    Q_PROPERTY(QStringList disabledPlugins READ disabledPlugins WRITE setDisabledPlugins NOTIFY disabledPluginsChanged)
public:
    /*!
     * \value PluginIdRole
     * \value IconNameRole
     * \value ActionDisplayRole
     */
    enum Roles {
        PluginIdRole = Qt::UserRole + 1,
        IconNameRole,
        ActionDisplayRole,
    };

    /*!
     * Constructs an alternatives model with the given \a parent.
     */
    explicit AlternativesModel(QObject *parent = nullptr);

    ~AlternativesModel() override;

    QJsonObject inputData() const;
    void setInputData(const QJsonObject &input);

    QString pluginType() const;
    void setPluginType(const QString &pluginType);

    QStringList disabledPlugins() const;
    void setDisabledPlugins(const QStringList &pluginIds);

    /*!
     * This shouldn't require to have the job actually running on the same process as the app.
     *
     * \a row specifies the alternative to be used.
     *
     * Returns the configuration instance that will offer the job.
     */
    Q_SCRIPTABLE Purpose::Configuration *configureJob(int row);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void inputDataChanged();
    void pluginTypeChanged();
    void disabledPluginsChanged();

private:
    PURPOSE_NO_EXPORT void initializeModel();

    AlternativesModelPrivate *const d_ptr;
    Q_DECLARE_PRIVATE(AlternativesModel)
};

}

#endif
