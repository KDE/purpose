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

#ifndef PURPOSE_CONFIGURATION_H
#define PURPOSE_CONFIGURATION_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <purpose/purpose_export.h>

class QJsonObject;
class KPluginMetaData;

namespace Purpose
{
struct ConfigurationPrivate;
class Job;

/**
 * @brief This class will be in charge of figuring out the job configuration
 *
 * Once it's figured out, it can proceed to create the job.
 *
 * The object will be destroyed as soon as the job finishes.
 */

class PURPOSE_EXPORT Configuration : public QObject
{
Q_OBJECT

/**
 * Tells whether there's still information to be provided, to be able to run
 * the job.
 *
 * @sa X-Purpose-MandatoryArguments and X-Purpose-AdditionalArguments
 */
Q_PROPERTY(bool isReady READ isReady NOTIFY dataChanged)

/**
 * Represents the data the job will have available to perform its task
 */
Q_PROPERTY(QJsonObject data READ data WRITE setData NOTIFY dataChanged)

/**
 * Specifies the arguments the config file and the job will be sharing
 */
Q_PROPERTY(QJsonArray neededArguments READ neededArguments CONSTANT)

/**
 * Specifies the qml source code to be used, to configure the current job.
 *
 * @sa PurposeWizard QtQuick component
 */
Q_PROPERTY(QUrl configSourceCode READ configSourceCode CONSTANT)
public:
    Configuration(const QJsonObject &inputData, const QJsonObject &pluginType, const KPluginMetaData &pluginInformation, QObject* parent = Q_NULLPTR);
    ~Configuration() Q_DECL_OVERRIDE;

    void setData(const QJsonObject& data);
    QJsonObject data() const;

    bool isReady() const;
    QJsonArray neededArguments() const;
    QUrl configSourceCode() const;

    Q_SCRIPTABLE Purpose::Job* createJob();

Q_SIGNALS:
    void dataChanged();

private:
    Q_DECLARE_PRIVATE(Configuration);
    ConfigurationPrivate* const d_ptr;
};

}

#endif // CONFIGURATION_H
