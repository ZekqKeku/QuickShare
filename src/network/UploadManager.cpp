#include "UploadManager.h"
#include "../utils/Archiver.h"
#include "../utils/NameGenerator.h"
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

UploadManager::UploadManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
    , m_currentFile(nullptr)
    , m_isUploading(false)
    , m_progress(0.0)
    , m_lastBytesSent(0)
    , m_lastTimestamp(0)
{
}

UploadManager::~UploadManager()
{
    cancelUpload();
}

void UploadManager::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void UploadManager::uploadFile(const QString &filePath)
{
    if (m_isUploading) {
        m_uploadQueue.append(filePath);
        return;
    }
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        emit uploadFailed(fileInfo.fileName(), "File does not exist");
        return;
    }
    
    m_currentFileName = fileInfo.fileName();
    emit currentFileNameChanged();
    
    m_currentFile = new QFile(filePath);
    if (!m_currentFile->open(QIODevice::ReadOnly)) {
        emit uploadFailed(m_currentFileName, "Cannot open file");
        delete m_currentFile;
        m_currentFile = nullptr;
        return;
    }
    
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant(QString("form-data; name=\"file\"; filename=\"%1\"").arg(m_currentFileName)));
    filePart.setBodyDevice(m_currentFile);
    m_currentFile->setParent(multiPart);
    
    multiPart->append(filePart);
    
    QNetworkRequest request(QUrl("https://pixeldrain.com/api/file"));
    
    if (!m_apiKey.isEmpty()) {
        QString credentials = QString(":%1").arg(m_apiKey);
        QByteArray base64Credentials = credentials.toUtf8().toBase64();
        request.setRawHeader("Authorization", "Basic " + base64Credentials);
    }
    
    m_isUploading = true;
    m_progress = 0.0;
    m_speed = "0 B/s";
    m_lastBytesSent = 0;
    m_uploadTimer.start();
    m_lastTimestamp = 0;
    
    emit isUploadingChanged();
    emit progressChanged();
    emit speedChanged();
    emit uploadStarted(m_currentFileName);
    
    m_currentReply = m_networkManager->post(request, multiPart);
    multiPart->setParent(m_currentReply);
    
    connect(m_currentReply, &QNetworkReply::uploadProgress,
            this, &UploadManager::onUploadProgress);
    connect(m_currentReply, &QNetworkReply::finished,
            this, &UploadManager::onUploadFinished);
    connect(m_currentReply, &QNetworkReply::errorOccurred,
            this, &UploadManager::onUploadError);
}

void UploadManager::uploadFiles(const QStringList &filePaths)
{
    if (filePaths.isEmpty()) return;
    
    QString fileToUpload;
    if (filePaths.size() > 1 || QFileInfo(filePaths.first()).isDir()) {
        QString zipName = "QuickShare_" + NameGenerator::generateRandomName() + ".zip";
        QString zipPath = QDir::tempPath() + "/" + zipName;
        fileToUpload = Archiver::createZip(filePaths, zipPath);
        if (fileToUpload.isEmpty()) {
            emit uploadFailed("Archive", "Failed to create ZIP archive");
            return;
        }
    } else {
        fileToUpload = filePaths.first();
    }
    
    uploadFile(fileToUpload);
}

void UploadManager::cancelUpload()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
    
    if (m_currentFile) {
        m_currentFile->close();
        m_currentFile = nullptr;
    }
    
    if (m_isUploading) {
        QString fileName = m_currentFileName;
        m_isUploading = false;
        m_progress = 0.0;
        emit isUploadingChanged();
        emit progressChanged();
        emit uploadCancelled(fileName);
    }
    
    m_uploadQueue.clear();
}

void UploadManager::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        m_progress = static_cast<qreal>(bytesSent) / static_cast<qreal>(bytesTotal);
        emit progressChanged();
        
        qint64 currentTime = m_uploadTimer.elapsed();
        qint64 timeDelta = currentTime - m_lastTimestamp;
        
        if (timeDelta >= 500) {
            qint64 bytesDelta = bytesSent - m_lastBytesSent;
            qint64 bytesPerSecond = (bytesDelta * 1000) / timeDelta;
            
            m_speed = formatSpeed(bytesPerSecond);
            emit speedChanged();
            
            m_lastBytesSent = bytesSent;
            m_lastTimestamp = currentTime;
        }
        
        emit uploadProgress(m_currentFileName, m_progress, m_speed);
    }
}

void UploadManager::onUploadFinished()
{
    if (!m_currentReply) return;
    
    if (m_currentReply->error() == QNetworkReply::NoError) {
        QByteArray responseData = m_currentReply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();
        
        if (jsonObj.contains("id")) {
            QString fileId = jsonObj["id"].toString();
            m_lastUploadUrl = QString("https://pixeldrain.com/u/%1").arg(fileId);
            emit lastUploadUrlChanged();
            emit uploadCompleted(m_currentFileName, m_lastUploadUrl);
        } else {
            emit uploadFailed(m_currentFileName, "Invalid server response");
        }
    }
    
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
    m_currentFile = nullptr;
    
    m_isUploading = false;
    m_progress = 0.0;
    emit isUploadingChanged();
    emit progressChanged();
    
    if (!m_uploadQueue.isEmpty()) {
        QString nextFile = m_uploadQueue.takeFirst();
        uploadFile(nextFile);
    }
}

void UploadManager::onUploadError(QNetworkReply::NetworkError error)
{
    if (error == QNetworkReply::OperationCanceledError) return;
    
    if (m_currentReply) {
        QString errorMsg = m_currentReply->errorString();
        emit uploadFailed(m_currentFileName, errorMsg);
    }
}

QString UploadManager::formatSpeed(qint64 bytesPerSecond)
{
    if (bytesPerSecond < 1024) return QString("%1 B/s").arg(bytesPerSecond);
    if (bytesPerSecond < 1024 * 1024) return QString("%1 KB/s").arg(bytesPerSecond / 1024.0, 0, 'f', 1);
    return QString("%1 MB/s").arg(bytesPerSecond / (1024.0 * 1024.0), 0, 'f', 2);
}

QString UploadManager::formatFileSize(qint64 bytes)
{
    if (bytes < 1024) return QString("%1 B").arg(bytes);
    if (bytes < 1024 * 1024) return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    if (bytes < 1024 * 1024 * 1024) return QString("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 2);
    return QString("%1 GB").arg(bytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
}
