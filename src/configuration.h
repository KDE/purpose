/*
    SPDX-FileCopyrightText: 2015 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef PURPOSE_CONFIGURATION_H
#define PURPOSE_CONFIGURATION_H

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QUrl>
#include <purpose/purpose_export.h>

class QJsonObject;
class KPluginMetaData;

namespace Purpose
{
class ConfigurationPrivate;
class Job;

/*!
 * \class Purpose::Configuration
 * \inheaderfile Purpose/Configuration
 * \inmodule Purpose
 *
 * \brief This class will be in charge of figuring out the job configuration.
 *
 * Once it's figured out, it can proceed to create the job.
 *
 * The object will be destroyed as soon as the job finishes.
 */

class PURPOSE_EXPORT Configuration : public QObject
{
    Q_OBJECT

    /*!
     * \property Purpose::Configuration::isReady
     *
     * Tells whether there's still information to be provided, to be able to run
     * the job.
     */
    Q_PROPERTY(bool isReady READ isReady NOTIFY dataChanged)

    /*!
     * \property Purpose::Configuration::data
     *
     * Represents the data the job will have available to perform its task.
     */
    Q_PROPERTY(QJsonObject data READ data WRITE setData NOTIFY dataChanged)

    /*!
     * \property Purpose::Configuration::neededArguments
     *
     * Specifies the arguments the config file and the job will be sharing.
     */
    Q_PROPERTY(QJsonArray neededArguments READ neededArguments CONSTANT)

    /*!
     * \property Purpose::Configuration::configSourceCode
     *
     * Specifies the QML source code to be used, to configure the current job.
     */
    Q_PROPERTY(QUrl configSourceCode READ configSourceCode CONSTANT)

    /*!
     * \property Purpose::Configuration::pluginTypeName
     *
     * Returns the plugin type name to display.
     */
    Q_PROPERTY(QString pluginTypeName READ pluginTypeName CONSTANT)

    /*!
     * \property Purpose::Configuration::pluginName
     *
     * Returns the plugin name to display.
     */
    Q_PROPERTY(QString pluginName READ pluginName CONSTANT)
public:
    /*!
     * \brief Constructs a new configuration.
     */
    Configuration(const QJsonObject &inputData,
                  const QString &pluginTypeName,
                  const QJsonObject &pluginType,
                  const KPluginMetaData &pluginInformation,
                  QObject *parent = nullptr);
    /*!
     * \brief Constructs a new configuration.
     */
    Configuration(const QJsonObject &inputData, const QString &pluginTypeName, const KPluginMetaData &pluginInformation, QObject *parent = nullptr);
    /*!
     * \brief Destroys the configuration.
     */
    ~Configuration() override;

    void setData(const QJsonObject &data);
    QJsonObject data() const;

    bool isReady() const;
    QJsonArray neededArguments() const;
    QUrl configSourceCode() const;

    /*!
     * Returns whether the job will be run in the same process.
     *
     * By default it will be true, unless the environment variable \c KDE_PURPOSE_LOCAL_JOBS is defined.
     */
    bool useSeparateProcess() const;

    /*!
     * \a separate will specify whether the process will be forced to execute
     * in-process or in a separate process.
     */
    void setUseSeparateProcess(bool separate);

    /*!
     * Returns the configured job ready to be started.
     *
     * Before calling it, make sure that all information has been filled by
     * checking \l isReady().
     */
    Q_SCRIPTABLE Purpose::Job *createJob();

    QString pluginName() const;

    QString pluginTypeName() const;

Q_SIGNALS:
    void dataChanged();

private:
    Q_DECLARE_PRIVATE(Configuration)
    ConfigurationPrivate *const d_ptr;
};

}

#endif // CONFIGURATION_H
