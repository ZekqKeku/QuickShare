#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>
#include <QClipboard>
#include <QMessageBox>
#include <QGuiApplication>
#include "../core/AppConfig.h"
#include "../utils/Archiver.h"
#include "../utils/NameGenerator.h"
#include "../utils/SystemUtils.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), uploader(new PixeldrainUploader(this)) {
    ui->setupUi(this);
    setAcceptDrops(true);

    ui->apiKeyEdit->setText(AppConfig::instance().apiKey());
    ui->hideDockCheckBox->setChecked(AppConfig::instance().isHideInDock());
    ui->autostartCheckBox->setChecked(AppConfig::instance().isAutoStart());

#ifdef Q_OS_MAC
    ui->hideDockCheckBox->setText("Hide in Dock");
#else
    ui->hideDockCheckBox->setText("Hide in Taskbar");
#endif

    ui->progressBar->hide();

    connect(ui->saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveSettings);
    connect(uploader, &PixeldrainUploader::progressChanged, this, &MainWindow::onUploadProgress);
    connect(uploader, &PixeldrainUploader::uploadFinished, this, &MainWindow::onUploadFinished);
    connect(uploader, &PixeldrainUploader::uploadError, this, &MainWindow::onUploadError);

    applyDockSettings();
    SystemUtils::registerContextMenu(true);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::applyDockSettings() {
    bool hide = AppConfig::instance().isHideInDock();
#ifdef Q_OS_MAC
    setMacDockIconVisible(!hide);
#else
    if (hide) {
        setWindowFlags(windowFlags() | Qt::Tool);
    } else {
        setWindowFlags(windowFlags() & ~Qt::Tool);
    }
    if (isVisible()) show();
#endif
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;

    QStringList files;
    for (const QUrl &url : urls) {
        files.append(url.toLocalFile());
    }

    startUpload(files);
}

void MainWindow::startUpload(const QStringList &files) {
    QString fileToUpload;

    if (files.size() > 1 || QFileInfo(files.first()).isDir()) {
        ui->label->setText("Zipping...");
        QString zipName = "QuickShare_" + NameGenerator::generateRandomName() + ".zip";
        QString zipPath = QDir::tempPath() + "/" + zipName;
        fileToUpload = Archiver::createZip(files, zipPath);
        if (fileToUpload.isEmpty()) {
            QMessageBox::critical(this, "Error", "Failed to create ZIP archive.");
            ui->label->setText("Drop files here");
            return;
        }
    } else {
        fileToUpload = files.first();
    }

    ui->label->setText("Uploading: " + QFileInfo(fileToUpload).fileName());
    
    qint64 fileSize = QFileInfo(fileToUpload).size();
    qint64 limit = 10LL * 1024 * 1024 * 1024;
    if (AppConfig::instance().apiKey().isEmpty() && fileSize > limit) {
        QMessageBox::warning(this, "Size Limit Exceeded", "File is larger than the 10GB limit for free accounts. Please configure an API key.");
        ui->label->setText("Drop files here");
        return;
    }

    ui->progressBar->setValue(0);
    ui->progressBar->show();
    uploader->uploadFile(fileToUpload);
    }

    void MainWindow::onUploadProgress(qint64 sent, qint64 total) {
    if (total > 0) {
        ui->progressBar->setValue(static_cast<int>((sent * 100) / total));
    }
    }

    void MainWindow::onUploadFinished(const QString &url) {
    ui->progressBar->hide();
    ui->label->setText("Done!");
    ui->urlResultEdit->setText(url);

    QGuiApplication::clipboard()->setText(url);
    ui->statusbar->showMessage("Link copied to clipboard", 3000);
    }

    void MainWindow::onUploadError(const QString &error) {
    ui->progressBar->hide();
    ui->label->setText("Error!");
    QMessageBox::warning(this, "Upload Error", error);
    }

    void MainWindow::onSaveSettings() {
        AppConfig::instance().setApiKey(ui->apiKeyEdit->text());
        AppConfig::instance().setHideInDock(ui->hideDockCheckBox->isChecked());
        AppConfig::instance().setAutoStart(ui->autostartCheckBox->isChecked());

        applyDockSettings();
        SystemUtils::setAutoStart(ui->autostartCheckBox->isChecked());
        SystemUtils::registerContextMenu(true);

        ui->statusbar->showMessage("Settings saved", 2000);
    }
