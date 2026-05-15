#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QClipboard>
#include <QEventLoop>
#include <QDateTime>
#include <iostream>
#include <string>
#include "gui/MainWindow.h"
#include "network/PixeldrainUploader.h"
#include "utils/Archiver.h"
#include "utils/NameGenerator.h"
#include "core/AppConfig.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

using namespace std;

void printLimitsWarning() {
    cout << "=============================================" << endl;
    cout << "WARNING: Pixeldrain API limits apply." << endl;
    cout << "Free users: 10GB per file, 20GB daily download limit." << endl;
    cout << "=============================================" << endl;
}

int main(int argc, char *argv[]) {
    bool isCli = argc > 1;
    for (int i = 1; i < argc; ++i) {
        QString arg = argv[i];
        if (arg.startsWith("-") || arg == "config") {
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
            ios::sync_with_stdio();
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

    QCommandLineOption fileOption({"f", "file"}, "File or folder to upload", "path");
    parser.addOption(fileOption);

    QCommandLineOption compressOption({"c", "compress"}, "Compress folder/files into ZIP");
    parser.addOption(compressOption);

    QCommandLineOption soloOption({"s", "solo"}, "Upload each file separately");
    parser.addOption(soloOption);

    QCommandLineOption apiOption({"a", "api"}, "Temporary API key", "key");
    parser.addOption(apiOption);

    parser.addPositionalArgument("command", "Command (config) or file path", "[command|path]");
    parser.addPositionalArgument("args", "Arguments for command", "[args...]");

    parser.process(app);

    const QStringList posArgs = parser.positionalArguments();

    if (posArgs.isEmpty() && !parser.isSet(fileOption) && !isCli) {
        MainWindow w;
        w.show();
        return app.exec();
    }

    if (posArgs.isEmpty() && !parser.isSet(fileOption) && isCli) {
        parser.showHelp();
        return 0;
    }

    printLimitsWarning();

    if (!posArgs.isEmpty() && posArgs[0] == "config") {
        if (posArgs.size() < 3) {
            cerr << "Usage: qs config <option> <value>" << endl;
            cerr << "Options: hide-in-taskbar, autostart, api" << endl;
            return 1;
        }
        QString option = posArgs[1];
        QString value = posArgs[2];

        if (option == "hide-in-taskbar") {
            AppConfig::instance().setHideInDock(value == "true");
            cout << "hide-in-taskbar set to " << value.toStdString() << endl;
        } else if (option == "autostart") {
            AppConfig::instance().setAutoStart(value == "true");
            cout << "autostart set to " << value.toStdString() << endl;
        } else if (option == "api") {
            AppConfig::instance().setApiKey(value);
            cout << "API key updated." << endl;
        } else {
            cerr << "Unknown config option: " << option.toStdString() << endl;
            return 1;
        }
        return 0;
    }

    QString apiKey = parser.value(apiOption);
    if (apiKey.isEmpty()) {
        apiKey = AppConfig::instance().apiKey();
    }

    if (apiKey.isEmpty()) {
        cerr << "Error: Pixeldrain API key is not set." << endl;
        cerr << "Please set it via 'qs config api <key>' or use '--api <key>' flag." << endl;
        cerr << "Get your API key at: https://pixeldrain.com/user/api_keys" << endl;
        return 1;
    }

    if (parser.isSet(compressOption) && parser.isSet(soloOption)) {
        cerr << "Error: Conflict in flags. Cannot use both -c (compress) and -s (solo)." << endl;
        return 1;
    }

    QStringList pathsToUpload;
    if (parser.isSet(fileOption)) {
        pathsToUpload.append(parser.value(fileOption));
    }
    for (const QString& arg : posArgs) {
        pathsToUpload.append(arg);
    }

    if (pathsToUpload.isEmpty()) {
        cerr << "Error: No files specified for upload." << endl;
        return 1;
    }

    bool compress = parser.isSet(compressOption);
    bool solo = parser.isSet(soloOption);

    bool hasDir = false;
    for (const QString& p : pathsToUpload) {
        if (QFileInfo(p).isDir()) {
            hasDir = true;
            break;
        }
    }

    if (solo && hasDir) {
        cout << "You have selected solo upload for a directory." << endl;
        cout << "Each file will be uploaded separately. Are you sure? [Y/n]: ";
        string resp;
        getline(cin, resp);
        if (!(resp == "" || resp == "y" || resp == "Y")) {
            cerr << "Upload cancelled." << endl;
            return 0;
        }
    }

    if (!compress && !solo) {
        if (hasDir || pathsToUpload.size() > 1) {
            cout << "Multiple files or a directory detected." << endl;
            cout << "Compress into ZIP? [Y/n]: ";
            string resp;
            getline(cin, resp);
            if (resp == "" || resp == "y" || resp == "Y") {
                compress = true;
            } else {
                cout << "Upload each file separately? [Y/n]: ";
                getline(cin, resp);
                if (resp == "" || resp == "y" || resp == "Y") {
                    solo = true;
                } else {
                    cerr << "Upload cancelled." << endl;
                    return 0;
                }
            }
        }
    }

    PixeldrainUploader uploader;
    if (!apiKey.isEmpty()) {
        uploader.setApiKey(apiKey);
    }

    QEventLoop loop;
    auto connectUploader = [&](PixeldrainUploader* up, const QString& fileName) {
        QObject::connect(up, &PixeldrainUploader::progressChanged, [fileName](qint64 sent, qint64 total) {
            if (total > 0) {
                int progress = static_cast<int>((sent * 100) / total);
                cout << "\r" << fileName.toStdString() << " Progress: " << progress << "% (" 
                          << (sent / 1024 / 1024) << "MB / " 
                          << (total / 1024 / 1024) << "MB)" << flush;
            }
        });
    };

    if (compress) {
        cout << "Creating archive..." << endl;
        QString zipName = "QuickShare_" + NameGenerator::generateRandomName() + ".zip";
        QString zipPath = QDir::tempPath() + "/" + zipName;
        QString fileToUpload = Archiver::createZip(pathsToUpload, zipPath);
        
        if (fileToUpload.isEmpty() || !QFileInfo::exists(fileToUpload)) {
            cerr << "Error: Archiving failed." << endl;
            return 1;
        }

        connectUploader(&uploader, QFileInfo(fileToUpload).fileName());
        QObject::connect(&uploader, &PixeldrainUploader::uploadFinished, [&](const QString &url) {
            cout << "\n=============================================" << endl;
            cout << "Link to your file:" << endl;
            cout << url.toStdString() << endl;
            cout << "=============================================" << endl;
            loop.quit();
        });
        QObject::connect(&uploader, &PixeldrainUploader::uploadError, [&](const QString &error) {
            cerr << "\nUpload error: " << error.toStdString() << endl;
            loop.quit();
        });
        uploader.uploadFile(fileToUpload);
        loop.exec();
    } else if (solo) {
        QString reportFileName = "qs-upload-data-" + QDateTime::currentDateTime().toString("dd-mm-yyyy-hh-mm-ss") + ".txt";
        QFile reportFile(reportFileName);
        if (!reportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            cerr << "Error: Could not create report file." << endl;
            return 1;
        }
        QTextStream out(&reportFile);

        QStringList allFiles;
        for (const QString& p : pathsToUpload) {
            QFileInfo fi(p);
            if (fi.isDir()) {
                QDirIterator it(p, QDir::Files, QDirIterator::Subdirectories);
                while (it.hasNext()) allFiles.append(it.next());
            } else {
                allFiles.append(p);
            }
        }
        cout << "Uploading " << allFiles.size() << " files..." << endl;
        for (const QString& f : allFiles) {
            PixeldrainUploader soloUp;
            if (!apiKey.isEmpty()) {
                soloUp.setApiKey(apiKey);
            }
            connectUploader(&soloUp, QFileInfo(f).fileName());
            
            QString resultUrl;
            bool success = false;
            QEventLoop soloLoop;
            
            QObject::connect(&soloUp, &PixeldrainUploader::uploadFinished, [&](const QString &url) {
                resultUrl = url;
                success = true;
                soloLoop.quit();
            });
            QObject::connect(&soloUp, &PixeldrainUploader::uploadError, [&](const QString &error) {
                cerr << "\nError uploading " << f.toStdString() << ": " << error.toStdString() << endl;
                soloLoop.quit();
            });
            
            soloUp.uploadFile(f);
            soloLoop.exec();
            
            if (success) {
                cout << "\nUploaded: " << resultUrl.toStdString() << endl;
                out << QFileInfo(f).fileName() << " - " << resultUrl << "\n";
            }
        }
        reportFile.close();
        cout << "Uploads finished. Report saved to " << reportFileName.toStdString() << endl;
    } else {
        QString fileToUpload = pathsToUpload.first();
        connectUploader(&uploader, QFileInfo(fileToUpload).fileName());
        QObject::connect(&uploader, &PixeldrainUploader::uploadFinished, [&](const QString &url) {
            cout << "\n=============================================" << endl;
            cout << "Link to your file:" << endl;
            cout << url.toStdString() << endl;
            cout << "=============================================" << endl;
            loop.quit();
        });
        QObject::connect(&uploader, &PixeldrainUploader::uploadError, [&](const QString &error) {
            cerr << "\nUpload error: " << error.toStdString() << endl;
            loop.quit();
        });
        uploader.uploadFile(fileToUpload);
        loop.exec();
    }

    return 0;
}
