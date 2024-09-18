/*
    SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef PURPOSEJOB_H
#define PURPOSEJOB_H

#include <KJob>
#include <QJsonArray>
#include <QJsonObject>
#include <QMimeData>
#include <QUrl>
#include <purpose/purpose_export.h>

/*!
 * \namespace Purpose
 * \inmodule Purpose
 */
namespace Purpose
{
class JobPrivate;

/*!
 * \class Purpose::Job
 * \inheaderfile Purpose/Job
 * \inmodule Purpose
 *
 * \brief Job that will actually perform the sharing.
 *
 * When start is called, the sharing process will start and when the job
 * emits finished, we'll know it's over.
 *
 * The start method called shouldn't be called before all data has been
 * filled in.
 */
class PURPOSE_EXPORT Job : public KJob
{
    Q_OBJECT
    /*!
     * \property Purpose::Job::data
     * Represents the data the job will have available to perform its task.
     */
    Q_PROPERTY(QJsonObject data READ data CONSTANT)

    /*!
     * \property Purpose::Job::output
     * Returns the output generated by the plugin.
     *
     * The information offered will depend on the plugin type.
     */
    Q_PROPERTY(QJsonObject output READ output WRITE setOutput NOTIFY outputChanged)
public:
    explicit Job(QObject *parent = nullptr);
    ~Job() override;

    /*!
     * \brief Should only be called after constructing.
     *
     * \internal
     */
    void setData(const QJsonObject &data);
    QJsonObject data() const;

    QJsonObject output() const;
    void setOutput(const QJsonObject &output);

Q_SIGNALS:
    void outputChanged(const QJsonObject &output);

private:
    Q_DECLARE_PRIVATE(Job)
    QScopedPointer<JobPrivate> const d_ptr;
};

}

#endif
