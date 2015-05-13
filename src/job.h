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

#ifndef PURPOSEJOB_H
#define PURPOSEJOB_H

#include <KJob>
#include <QJsonArray>
#include <QJsonObject>
#include <QMimeData>
#include <QUrl>
#include <purpose/purpose_export.h>

namespace Purpose
{

class JobPrivate;

/**
 * @brief Job that will actually perform the sharing
 *
 * When start is called, the sharing process will start and when the job
 * emits finished, we'll know it's over.
 *
 * The start method called shouldn't be called before all data has been
 * filled in. isReady can be used to check whether it's all ready to go,
 * these arguments will have to be filled by the file provided by
 * configSourceCode() and should end up defining all the arguments defined
 * by neededArguments.
 */
class PURPOSE_EXPORT Job : public KJob
{
Q_OBJECT
/**
 * Represents the data the job will have available to perform its task
 */
Q_PROPERTY(QJsonObject data READ data WRITE setData NOTIFY dataChanged)

/**
 * Tells whether there's still information to be provided, to be able to run
 * the job.
 *
 * @sa X-Purpose-MandatoryArguments and X-Purpose-AdditionalArguments
 */
Q_PROPERTY(bool isReady READ isReady NOTIFY dataChanged)

/**
 * Specifies the qml source code to be used, to configure the current job.
 *
 * @sa ShareWizard QtQuick component
 */
Q_PROPERTY(QUrl configSourceCode READ configSourceCode CONSTANT)

/**
 * Specifies the arguments the config file and the job will be sharing
 */
Q_PROPERTY(QJsonArray neededArguments READ neededArguments CONSTANT)
public:
    Job(QObject* parent = 0);
    virtual ~Job();

    void setData(const QJsonObject& data);
    QJsonObject data() const;

    bool isReady() const;
    QJsonArray neededArguments() const;
    virtual QUrl configSourceCode() const = 0;

    /**
     * @internal
     */
    void setInboundArguments(const QJsonValue& args);

    /**
     * @internal
     */
    void setConfigurationArguments(const QJsonValue& args);


Q_SIGNALS:
    void output(const QJsonObject& output);
    void dataChanged();

private:
    Q_DECLARE_PRIVATE(Job);
    QScopedPointer<JobPrivate> const d_ptr;
};

}

#endif
