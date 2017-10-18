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

#ifndef PURPOSEALTERNATIVESMODEL_H
#define PURPOSEALTERNATIVESMODEL_H

#include <QAbstractListModel>
#include <QJsonObject>

#include <purpose/purpose_export.h>

namespace Purpose
{
class Configuration;
class AlternativesModelPrivate;

/**
 * @short Interface for client applications to share data
 *
 * Lists all the alternatives to share a specified type of data then provides
 * a method to trigger a job.
 */
class PURPOSE_EXPORT AlternativesModel : public QAbstractListModel
{
Q_OBJECT
Q_PROPERTY(QString pluginType READ pluginType WRITE setPluginType NOTIFY pluginTypeChanged)
Q_PROPERTY(QJsonObject inputData READ inputData WRITE setInputData NOTIFY inputDataChanged)
Q_PROPERTY(QStringList disabledPlugins READ disabledPlugins WRITE setDisabledPlugins NOTIFY disabledPluginsChanged)
public:
    enum Roles {
        PluginIdRole = Qt::UserRole+1,
        IconNameRole
    };

    AlternativesModel(QObject* parent = Q_NULLPTR);
    virtual ~AlternativesModel();

    QJsonObject inputData() const;
    void setInputData(const QJsonObject& input);

    QString pluginType() const;
    void setPluginType(const QString& pluginType);

    QStringList disabledPlugins() const;
    void setDisabledPlugins(const QStringList& pluginIds);

    /**
     * This shouldn't require to have the job actually running on the same process as the app.
     *
     * @param row specifies the alternative to be used
     * @param data specifies the data to have shared
     *
     * @returns the configuration instance that will offer the job.
     */
    Q_SCRIPTABLE Purpose::Configuration* configureJob(int row);

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QHash<int,QByteArray> roleNames() const override;

Q_SIGNALS:
    void inputDataChanged();
    void pluginTypeChanged();
    void disabledPluginsChanged();

private:
    void initializeModel();

    AlternativesModelPrivate *const d_ptr;
    Q_DECLARE_PRIVATE(AlternativesModel)
};

}

#endif
