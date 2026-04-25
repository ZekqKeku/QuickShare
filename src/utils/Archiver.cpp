#include "Archiver.h"
#include <QProcess>
#include <QFileInfo>
#include <QDebug>

QString Archiver::createZip(const QStringList &paths, const QString &outputZipPath) {
    if (paths.isEmpty()) return "";

    QProcess process;
    QStringList arguments;
    arguments << "-r" << outputZipPath;

    for (const QString &path : paths) {
        QFileInfo info(path);

        process.setWorkingDirectory(info.absolutePath());
        arguments << info.fileName();
    }

#ifdef Q_OS_WIN
    QStringList winArguments;
    winArguments << "-a" << "-c" << "-f" << outputZipPath;
    for (const QString &path : paths) {
        QFileInfo info(path);
        winArguments << info.fileName();
    }
    process.start("tar", winArguments);
#else
    process.start("zip", arguments);
#endif

    if (!process.waitForFinished()) {
        qDebug() << "Archiver error:" << process.errorString();
        return "";
    }

    if (process.exitCode() != 0) {
        qDebug() << "Archiver exit code:" << process.exitCode() << process.readAllStandardError();
        return "";
    }

    return outputZipPath;
}
