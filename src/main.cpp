#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QClipboard>
#include <QEventLoop>
#include "gui/MainWindow.h"
#include "network/PixeldrainUploader.h"
#include "utils/Archiver.h"
#include "utils/NameGenerator.h"
#include "core/AppConfig.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(QString("%1-%2").arg(APP_VERSION, APP_STATUS));

    QCommandLineParser parser;
    parser.setApplicationDescription(APP_DESCRIPTION);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("files", "Files to upload", "[files...]");

    parser.process(app);

    const QStringList files = parser.positionalArguments();

    if (files.isEmpty()) {
        MainWindow w;
        w.show();
        return app.exec();
    } else {

        qDebug() << "Headless mode started for:" << files;

        QString fileToUpload;
        if (files.size() > 1 || QFileInfo(files.first()).isDir()) {
            QString zipName = "QuickShare_" + NameGenerator::generateRandomName() + ".zip";
            QString zipPath = QDir::tempPath() + "/" + zipName;
            fileToUpload = Archiver::createZip(files, zipPath);
        } else {
            fileToUpload = files.first();
        }

        if (fileToUpload.isEmpty() || !QFileInfo::exists(fileToUpload)) {
            qCritical() << "Error: File does not exist or archiving failed.";
            return 1;
        }

        qint64 fileSize = QFileInfo(fileToUpload).size();
        qint64 limit = 10LL * 1024 * 1024 * 1024;
        if (AppConfig::instance().apiKey().isEmpty() && fileSize > limit) {
            qCritical() << "Error: File is larger than the 10GB limit for free accounts. Please configure an API key.";
            return 1;
        }

        PixeldrainUploader uploader;
        QEventLoop loop;

        QObject::connect(&uploader, &PixeldrainUploader::uploadFinished, [&](const QString &url) {
            qDebug() << "Upload finished:" << url;
            QGuiApplication::clipboard()->setText(url);
            qDebug() << "Link copied to clipboard.";
            loop.quit();
        });

        QObject::connect(&uploader, &PixeldrainUploader::uploadError, [&](const QString &error) {
            qCritical() << "Upload error:" << error;
            loop.quit();
        });

        uploader.uploadFile(fileToUpload);
        loop.exec();

        return 0;
    }
}
