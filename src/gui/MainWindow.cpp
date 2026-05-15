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
#include "../core/LanguageManager.h"
#include "../utils/Archiver.h"
#include "../utils/NameGenerator.h"
#include "../utils/SystemUtils.h"

using namespace Qt;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), uploader(new PixeldrainUploader(this)) {
    ui->setupUi(this);
    setAcceptDrops(true);

    ui->apiKeyEdit->setText(AppConfig::instance().apiKey());
    ui->hideDockCheckBox->setChecked(AppConfig::instance().isHideInDock());
    ui->autostartCheckBox->setChecked(AppConfig::instance().isAutoStart());

    if (AppConfig::instance().language() == "plPL") {
        ui->languageComboBox->setCurrentIndex(1);
    } else {
        ui->languageComboBox->setCurrentIndex(0);
    }

    retranslateUi();

    ui->progressBar->hide();
    ui->statusbar->showMessage(QString("QuickShare v%1 | Connected to Pixeldrain").arg(APP_VERSION));

    connect(ui->saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveSettings);
    connect(uploader, &PixeldrainUploader::progressChanged, this, &MainWindow::onUploadProgress);
    connect(uploader, &PixeldrainUploader::uploadFinished, this, &MainWindow::onUploadFinished);
    connect(uploader, &PixeldrainUploader::uploadError, this, &MainWindow::onUploadError);

    applyDockSettings();
    SystemUtils::registerContextMenu(true);
}

void MainWindow::retranslateUi() {
    auto& lm = LanguageManager::instance();
    ui->label->setText(lm.getString("drop_files_here"));
    ui->apiKeyEdit->setPlaceholderText(lm.getString("api_key"));
    ui->autostartCheckBox->setText(lm.getString("autostart"));
    ui->saveBtn->setText(lm.getString("save"));
    ui->urlResultEdit->setPlaceholderText(lm.getString("url_placeholder"));
    
    ui->tabWidget->setTabText(0, lm.getString("upload_tab"));
    ui->tabWidget->setTabText(1, lm.getString("settings_tab"));
    ui->label_apiKey->setText(lm.getString("api_key_label"));
    ui->label_lang->setText(lm.getString("language_label"));
    
#ifdef Q_OS_MAC
    ui->hideDockCheckBox->setText(lm.getString("hide_in_dock"));
#else
    ui->hideDockCheckBox->setText(lm.getString("hide_in_taskbar"));
#endif
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
        setWindowFlags(windowFlags() | Tool);
    } else {
        setWindowFlags(windowFlags() & ~Tool);
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
    auto& lm = LanguageManager::instance();
    QString fileToUpload;

    if (files.size() > 1 || QFileInfo(files.first()).isDir()) {
        ui->label->setText(lm.getString("zipping"));
        QString zipName = "QuickShare_" + NameGenerator::generateRandomName() + ".zip";
        QString zipPath = QDir::tempPath() + "/" + zipName;
        fileToUpload = Archiver::createZip(files, zipPath);
        if (fileToUpload.isEmpty()) {
            QMessageBox::critical(this, lm.getString("error"), "Failed to create ZIP archive.");
            ui->label->setText(lm.getString("drop_files_here"));
            return;
        }
    } else {
        fileToUpload = files.first();
    }

    ui->label->setText(lm.getString("uploading") + QFileInfo(fileToUpload).fileName());
    
    qint64 fileSize = QFileInfo(fileToUpload).size();
    qint64 limit = 10LL * 1024 * 1024 * 1024;
    if (AppConfig::instance().apiKey().isEmpty() && fileSize > limit) {
        QMessageBox::warning(this, "Limit", lm.getString("size_limit_warning"));
        ui->label->setText(lm.getString("drop_files_here"));
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
    auto& lm = LanguageManager::instance();
    ui->progressBar->hide();
    ui->label->setText(lm.getString("done"));
    ui->urlResultEdit->setText(url);

    QGuiApplication::clipboard()->setText(url);
    ui->statusbar->showMessage(lm.getString("link_copied"), 3000);
}

void MainWindow::onUploadError(const QString &error) {
    auto& lm = LanguageManager::instance();
    ui->progressBar->hide();
    ui->label->setText(lm.getString("error"));
    QMessageBox::warning(this, lm.getString("upload_error"), error);
}

void MainWindow::onSaveSettings() {
    auto& lm = LanguageManager::instance();
    QString selectedLang = (ui->languageComboBox->currentIndex() == 1) ? "plPL" : "enUS";
    lm.setLanguage(selectedLang);
    retranslateUi();

    AppConfig::instance().setApiKey(ui->apiKeyEdit->text());
    AppConfig::instance().setHideInDock(ui->hideDockCheckBox->isChecked());
    AppConfig::instance().setAutoStart(ui->autostartCheckBox->isChecked());

    applyDockSettings();
    SystemUtils::setAutoStart(ui->autostartCheckBox->isChecked());
    SystemUtils::registerContextMenu(true);

    ui->statusbar->showMessage(lm.getString("settings_saved"), 2000);
}