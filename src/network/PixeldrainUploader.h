#ifndef PIXELDRAINUPLOADER_H
#define PIXELDRAINUPLOADER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class PixeldrainUploader : public QObject {
    Q_OBJECT
public:
    explicit PixeldrainUploader(QObject *parent = nullptr);
    void uploadFile(const QString &filePath);

signals:
    void progressChanged(qint64 bytesSent, qint64 bytesTotal);
    void uploadFinished(const QString &url);
    void uploadError(const QString &error);

private slots:
    void onUploadFinished();

private:
    QNetworkAccessManager *manager;
};

#endif
