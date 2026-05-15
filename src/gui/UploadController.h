#ifndef UPLOADCONTROLLER_H
#define UPLOADCONTROLLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>
#include <QElapsedTimer>
#include <QStringList>
#include <QTimer>

class UploadController : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(bool isUploading READ isUploading NOTIFY isUploadingChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString speed READ speed NOTIFY speedChanged)
    Q_PROPERTY(QString currentFileName READ currentFileName NOTIFY currentFileNameChanged)
    Q_PROPERTY(QString currentFileSize READ currentFileSize NOTIFY currentFileSizeChanged)
    Q_PROPERTY(QString lastUploadUrl READ lastUploadUrl NOTIFY lastUploadUrlChanged)
    Q_PROPERTY(bool isApiValid READ isApiValid NOTIFY isApiValidChanged)
    
public:
    explicit UploadController(QObject *parent = nullptr);
    ~UploadController();
    
    bool isUploading() const { return m_isUploading; }
    qreal progress() const { return m_progress; }
    QString speed() const { return m_speed; }
    QString currentFileName() const { return m_currentFileName; }
    QString currentFileSize() const { return m_currentFileSize; }
    QString lastUploadUrl() const { return m_lastUploadUrl; }
    bool isApiValid() const { return m_isApiValid; }
    
public slots:
    void uploadFile(const QString &filePath);
    void uploadFiles(const QStringList &filePaths);
    void cancelUpload();
    void setApiKey(const QString &apiKey);
    void validateApiKey();
    void clearLastUpload();
    
signals:
    void isUploadingChanged();
    void progressChanged();
    void speedChanged();
    void currentFileNameChanged();
    void currentFileSizeChanged();
    void lastUploadUrlChanged();
    void isApiValidChanged();
    
    void uploadStarted(const QString &fileName);
    void uploadProgress(const QString &fileName, qreal progress, const QString &speed);
    void uploadCompleted(const QString &fileName, const QString &url);
    void uploadFailed(const QString &fileName, const QString &error);
    void uploadCancelled(const QString &fileName);
    
private slots:
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void onUploadFinished();
    void onUploadError(QNetworkReply::NetworkError error);
    void onValidationTimer();
    
private:
    QString formatSpeed(qint64 bytesPerSecond);
    QString formatFileSize(qint64 bytes);
    
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    QFile *m_currentFile;
    QElapsedTimer m_uploadTimer;
    QTimer *m_validationTimer;
    
    QString m_apiKey;
    bool m_isUploading;
    bool m_isApiValid;
    qreal m_progress;
    QString m_speed;
    QString m_currentFileName;
    QString m_currentFileSize;
    QString m_lastUploadUrl;
    qint64 m_lastBytesSent;
    qint64 m_lastTimestamp;
    
    QStringList m_uploadQueue;
};

#endif
