#include "PixeldrainUploader.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include "../core/AppConfig.h"

PixeldrainUploader::PixeldrainUploader(QObject *parent)
    : QObject(parent), manager(new QNetworkAccessManager(this)) {}

void PixeldrainUploader::uploadFile(const QString &filePath) {
    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        emit uploadError("Cannot open file: " + filePath);
        delete file;
        return;
    }

    QFileInfo fileInfo(filePath);
    QUrl url("https://pixeldrain.com/api/file/" + fileInfo.fileName());
    QNetworkRequest request(url);

    QString apiKey = AppConfig::instance().apiKey();
    if (!apiKey.isEmpty()) {
        QString auth = "Basic " + QByteArray(":" + apiKey.toUtf8()).toBase64();
        request.setRawHeader("Authorization", auth.toUtf8());
    }

    QNetworkReply *reply = manager->put(request, file);
    file->setParent(reply); 

    connect(reply, &QNetworkReply::uploadProgress, this, &PixeldrainUploader::progressChanged);
    connect(reply, &QNetworkReply::finished, this, &PixeldrainUploader::onUploadFinished);
}

void PixeldrainUploader::onUploadFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObj = jsonDoc.object();

        if (jsonObj.contains("id")) {
            QString id = jsonObj["id"].toString();
            QString downloadUrl = "https://pixeldrain.com/u/" + id;
            emit uploadFinished(downloadUrl);
        } else {
            emit uploadError("Unexpected server response: " + QString(response));
        }
    } else {
        emit uploadError("Network error: " + reply->errorString());
    }

    reply->deleteLater();
}
