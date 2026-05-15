#include "CliHandler.h"
#include <QCommandLineParser>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QEventLoop>
#include <iostream>
#include "../core/SettingsManager.h"
#include "../network/PixeldrainUploader.h"
#include "../utils/Archiver.h"
#include "../utils/NameGenerator.h"

using namespace std;

CliHandler::CliHandler(QObject *parent) : QObject(parent) {}

void CliHandler::printLimitsWarning() {
    cout << "=============================================" << endl;
    cout << "WARNING: Pixeldrain API limits apply." << endl;
    cout << "Free users: 10GB per file, 20GB daily download limit." << endl;
    cout << "=============================================" << endl;
}

int CliHandler::execute(const QStringList &args) {
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
    
    if (!parser.parse(args)) {
        cerr << parser.errorText().toStdString() << endl;
        return 1;
    }

    if (parser.isSet("help")) {
        cout << parser.helpText().toStdString() << endl;
        return 0;
    }

    if (parser.isSet("version")) {
        cout << APP_NAME << " " << APP_VERSION << endl;
        return 0;
    }

    const QStringList posArgs = parser.positionalArguments();
    
    if (!posArgs.isEmpty() && posArgs[0] == "config") {
        return handleConfig(posArgs);
    }

    printLimitsWarning();
    return handleUpload(posArgs, parser.isSet(compressOption), parser.isSet(soloOption), 
                        parser.value(apiOption), parser.value(fileOption));
}

int CliHandler::handleConfig(const QStringList &posArgs) {
    SettingsManager settingsManager;
    if (posArgs.size() < 3) {
        cerr << "Usage: qs config <option> <value>" << endl;
        cerr << "Options: api, autostart, hide-in-taskbar" << endl;
        return 1;
    }
    QString option = posArgs[1];
    QString value = posArgs[2];

    if (option == "hide-in-taskbar") {
        settingsManager.setMinimizeToTray(value == "true");
        settingsManager.save();
        cout << "hide-in-taskbar set to " << value.toStdString() << endl;
    } else if (option == "autostart") {
        settingsManager.setStartWithSystem(value == "true");
        settingsManager.save();
        cout << "autostart set to " << value.toStdString() << endl;
    } else if (option == "api") {
        settingsManager.setApiKey(value);
        settingsManager.save();
        cout << "API key updated." << endl;
    } else {
        cerr << "Unknown config option: " << option.toStdString() << endl;
        return 1;
    }
    return 0;
}

int CliHandler::handleUpload(const QStringList &posArgs, bool compress, bool solo, const QString &apiFlag, const QString &fileFlag) {
    SettingsManager settingsManager;
    QString apiKey = !apiFlag.isEmpty() ? apiFlag : settingsManager.apiKey();
    
    if (apiKey.isEmpty()) {
        cerr << "Error: Pixeldrain API key is not set. To upload files, you need an API key." << endl;
        cerr << "Get your API key at: https://pixeldrain.com/user/api_keys" << endl;
        cerr << "Then set it using: qs config api <key>" << endl;
        return 1;
    }

    if (compress && solo) {
        cerr << "Error: Conflict in flags. Cannot use both -c and -s." << endl;
        return 1;
    }

    QStringList pathsToUpload;
    if (!fileFlag.isEmpty()) pathsToUpload.append(fileFlag);
    for (const QString& arg : posArgs) {
        if (arg != "config") pathsToUpload.append(arg);
    }

    if (pathsToUpload.isEmpty()) {
        cerr << "Error: No files to upload." << endl;
        return 1;
    }

    for (const QString& p : pathsToUpload) {
        if (!QFileInfo::exists(p)) {
            cerr << "Error: File or directory does not exist: " << p.toStdString() << endl;
            return 1;
        }
    }

    bool hasDir = false;
    for (const QString& p : pathsToUpload) if (QFileInfo(p).isDir()) hasDir = true;

    if (!compress && !solo && (hasDir || pathsToUpload.size() > 1)) {
        cout << "Multiple files/directory detected. Compress into ZIP? [Y/n]: ";
        string resp; getline(cin, resp);
        if (resp == "" || resp == "y" || resp == "Y") compress = true;
        else {
            cout << "Upload each file separately? [Y/n]: ";
            getline(cin, resp);
            if (resp == "" || resp == "y" || resp == "Y") solo = true;
            else { cerr << "Upload cancelled." << endl; return 0; }
        }
    }

    PixeldrainUploader uploader;
    uploader.setApiKey(apiKey);
    QEventLoop loop;

    auto connectLog = [&](const QString& name) {
        QObject::connect(&uploader, &PixeldrainUploader::progressChanged, [name](qint64 s, qint64 t) {
            if (t > 0) cout << "\r" << name.toStdString() << ": " << (s*100/t) << "%" << flush;
        });
    };

    if (compress) {
        QString zipPath = QDir::tempPath() + "/QuickShare_" + NameGenerator::generateRandomName() + ".zip";
        QString file = Archiver::createZip(pathsToUpload, zipPath);
        if (file.isEmpty()) return 1;
        connectLog(QFileInfo(file).fileName());
        QObject::connect(&uploader, &PixeldrainUploader::uploadFinished, [&](const QString &url) {
            cout << "\n=============================================\nLink to your file:\n" << url.toStdString() << "\n=============================================" << endl;
            loop.quit();
        });
        QObject::connect(&uploader, &PixeldrainUploader::uploadError, [&](const QString &err) {
            cerr << "\nUpload error: " << err.toStdString() << endl;
            loop.quit();
        });
        uploader.uploadFile(file);
        loop.exec();
    } else if (solo) {
        QString reportFileName = "qs-upload-data(" + QDateTime::currentDateTime().toString("dd-MM-yyyy-hh-mm-ss") + ").txt";
        QFile reportFile(reportFileName);
        bool reportOpen = reportFile.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream reportStream(&reportFile);

        for (const QString& p : pathsToUpload) {
            cout << "Uploading " << QFileInfo(p).fileName().toStdString() << "..." << endl;
            uploader.uploadFile(p); 
            QEventLoop fileLoop;
            auto conn1 = QObject::connect(&uploader, &PixeldrainUploader::uploadFinished, [&](const QString &url) {
                cout << "Link: " << url.toStdString() << endl;
                if (reportOpen) {
                    reportStream << QFileInfo(p).fileName() << " - " << url << "\n";
                }
                fileLoop.quit();
            });
            auto conn2 = QObject::connect(&uploader, &PixeldrainUploader::uploadError, [&](const QString &err) {
                cerr << "Error: " << err.toStdString() << endl;
                fileLoop.quit();
            });
            fileLoop.exec();
            QObject::disconnect(conn1);
            QObject::disconnect(conn2);
        }
        if (reportOpen) {
            reportFile.close();
            cout << "Upload report saved to: " << reportFileName.toStdString() << endl;
        }
    } else {
        connectLog(QFileInfo(pathsToUpload.first()).fileName());
        QObject::connect(&uploader, &PixeldrainUploader::uploadFinished, [&](const QString &url) {
            cout << "\nLink: " << url.toStdString() << endl;
            loop.quit();
        });
        QObject::connect(&uploader, &PixeldrainUploader::uploadError, [&](const QString &err) {
            cerr << "\nUpload error: " << err.toStdString() << endl;
            loop.quit();
        });
        uploader.uploadFile(pathsToUpload.first());
        loop.exec();
    }

    return 0;
}
