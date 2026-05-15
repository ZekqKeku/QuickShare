#include "Archiver.h"
#include <QProcess>
#include <QFileInfo>
#include <QDebug>
#include <QDir>

QString Archiver::createZip(const QStringList &paths, const QString &outputZipPath) {
    if (paths.isEmpty()) return "";

    QProcess process;
    QStringList arguments;

#ifdef Q_OS_WIN
    arguments << "-a" << "-c" << "-f" << QDir::toNativeSeparators(outputZipPath);
    for (const QString &path : paths) {
        arguments << QDir::toNativeSeparators(path);
    }
    process.start("tar", arguments);
#else
    arguments << "-r" << outputZipPath; 
    for (const QString &path : paths) {
        arguments << path;
    }
    process.start("zip", arguments);
#endif

    if (!process.waitForFinished(30000)) { 
        qDebug() << "Archiver error or timeout:" << process.errorString();
        process.kill();
        return "";
    }

    if (process.exitCode() != 0) {
        qDebug() << "Archiver exit code:" << process.exitCode() << process.readAllStandardError();
        return "";
    }

    return outputZipPath;
}
