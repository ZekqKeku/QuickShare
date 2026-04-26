#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QClipboard>
#include <QEventLoop>
#include <iostream>
#include "gui/MainWindow.h"
#include "network/PixeldrainUploader.h"
#include "utils/Archiver.h"
#include "utils/NameGenerator.h"
#include "core/AppConfig.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
    bool isCli = argc > 1;
    for (int i = 1; i < argc; ++i) {
        QString arg = argv[i];
        if (arg == "-h" || arg == "--help" || arg == "-v" || arg == "--version") {
            isCli = true;
            break;
        }
    }

#ifdef Q_OS_WIN
    if (isCli) {
        if (AttachConsole(ATTACH_PARENT_PROCESS)) {
            FILE* fp;
            freopen_s(&fp, "CONOUT$", "w", stdout);
            freopen_s(&fp, "CONOUT$", "w", stderr);
            std::ios::sync_with_stdio();
        }
    }
#endif

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

    if (files.isEmpty() && !isCli) {
        MainWindow w;
        w.show();
        return app.exec();
    } else if (files.isEmpty() && isCli) {
        parser.showHelp();
        return 0;
    } else {
        std::cout << "QuickShare " << APP_VERSION << " " << APP_STATUS << std::endl;
        std::cout << "Uploading: " << files.join(", ").toStdString() << std::endl;

        QString fileToUpload;
        if (files.size() > 1 || QFileInfo(files.first()).isDir()) {
            std::cout << "Creating archive..." << std::endl;
            QString zipName = "QuickShare_" + NameGenerator::generateRandomName() + ".zip";
            QString zipPath = QDir::tempPath() + "/" + zipName;
            fileToUpload = Archiver::createZip(files, zipPath);
        } else {
            fileToUpload = files.first();
        }

        if (fileToUpload.isEmpty() || !QFileInfo::exists(fileToUpload)) {
            std::cerr << "Error: File does not exist or archiving failed." << std::endl;
            return 1;
        }

        qint64 fileSize = QFileInfo(fileToUpload).size();
        qint64 limit = 10LL * 1024 * 1024 * 1024;
        if (AppConfig::instance().apiKey().isEmpty() && fileSize > limit) {
            std::cerr << "Error: File is larger than the 10GB limit for free accounts. Please configure an API key." << std::endl;
            return 1;
        }

        PixeldrainUploader uploader;
        QEventLoop loop;

        QObject::connect(&uploader, &PixeldrainUploader::progressChanged, [](qint64 sent, qint64 total) {
            if (total > 0) {
                int progress = static_cast<int>((sent * 100) / total);
                std::cout << "\rProgress: " << progress << "% (" 
                          << (sent / 1024 / 1024) << "MB / " 
                          << (total / 1024 / 1024) << "MB)" << std::flush;
            }
        });

        QObject::connect(&uploader, &PixeldrainUploader::uploadFinished, [&](const QString &url) {
            std::cout << "\nUpload finished successfully!" << std::endl;
            std::cout << "URL: " << url.toStdString() << std::endl;
            QGuiApplication::clipboard()->setText(url);
            std::cout << "Link copied to clipboard." << std::endl;
            loop.quit();
        });

        QObject::connect(&uploader, &PixeldrainUploader::uploadError, [&](const QString &error) {
            std::cerr << "\nUpload error: " << error.toStdString() << std::endl;
            loop.quit();
        });

        uploader.uploadFile(fileToUpload);
        loop.exec();

        return 0;
    }
}
