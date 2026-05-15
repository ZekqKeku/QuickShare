#ifndef CLIHANDLER_H
#define CLIHANDLER_H

#include <QStringList>
#include <QObject>

class CliHandler : public QObject {
    Q_OBJECT
public:
    explicit CliHandler(QObject *parent = nullptr);
    int execute(const QStringList &args);

private:
    void printLimitsWarning();
    int handleConfig(const QStringList &posArgs);
    int handleUpload(const QStringList &posArgs, bool compress, bool solo, const QString &apiFlag, const QString &fileFlag);
};

#endif
