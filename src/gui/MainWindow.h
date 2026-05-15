#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../network/PixeldrainUploader.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void onUploadProgress(qint64 sent, qint64 total);
    void onUploadFinished(const QString &url);
    void onUploadError(const QString &error);
    void onSaveSettings();

private:
    Ui::MainWindow *ui;
    PixeldrainUploader *uploader;
    void startUpload(const QStringList &files);
    void applyDockSettings();
    void retranslateUi();
};

#endif
