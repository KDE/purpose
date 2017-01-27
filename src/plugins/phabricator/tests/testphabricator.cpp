#include <QDebug>
#include <QString>
#include <QCoreApplication>
#include <QCommandLineParser>

#include "../phabricatorjobs.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString projectName;
    QString diffID;
    QString patchFile;

    app.setApplicationName(QStringLiteral("testphabricator"));
    QCommandLineParser parser;
    const QCommandLineOption projectNameOption(QStringLiteral("project"),
        QStringLiteral("set the project name"),
        QStringLiteral("project"), projectName);
    const QCommandLineOption diffIDOption(QStringLiteral("ID"),
        QStringLiteral("set the revision ID"),
        QStringLiteral("ID"), diffID);
    const QCommandLineOption patchFileOption(QStringLiteral("patch"),
        QStringLiteral("the patch to upload"),
        QStringLiteral("patch"), patchFile);
    const QCommandLineOption listOption(QStringLiteral("list"), QStringLiteral("list your open differential revisions"));
    parser.addOption(projectNameOption);
    parser.addOption(diffIDOption);
    parser.addOption(patchFileOption);
    parser.addOption(listOption);
    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(app);
    if (parser.isSet(listOption)) {
        Phabricator::DiffRevList diffList;
        if (diffList.error()) {
            qCritical() << "Error creating diffList:" << diffList.errorString() << ";" << diffList.error();
        } else {
            diffList.exec();
            if (diffList.error()) {
                qCritical() << "Error getting diffList:" << diffList.errorString() << ";" << diffList.error();
            } else {
                qWarning() << "Open differential revisions:" << diffList.reviews();
            }
        }
    } else {
        if (parser.isSet(projectNameOption)) {
            projectName = parser.value(projectNameOption);
        }
        if (parser.isSet(diffIDOption)) {
            diffID = parser.value(diffIDOption);
        }
        if (parser.isSet(patchFileOption)) {
            patchFile = parser.value(patchFileOption);
        }
    }
    exit(0);
}
