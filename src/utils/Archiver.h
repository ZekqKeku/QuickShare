#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <QString>
#include <QStringList>

class Archiver {
public:
    static QString createZip(const QStringList &paths, const QString &outputZipPath);
};

#endif
