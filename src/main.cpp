#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QIcon>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QEventLoop>
#include <QDirIterator>
#include <iostream>

#include "core/SettingsManager.h"
#include "network/UploadManager.h"
#include "network/PixeldrainUploader.h"
#include "utils/Clipboard.h"
#include "utils/SystemUtils.h"
#include "utils/Archiver.h"
#include "utils/NameGenerator.h"

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

int main(int argc, char *argv[])
{
    bool hasCliFlags = false;
    for (int i = 1; i < argc; ++i) {
        QString arg = argv[i];
        if (arg.startsWith("-") || arg == "config") {
            hasCliFlags = true;
            break;
        }
    }

#ifdef Q_OS_WIN
    if (hasCliFlags) {
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
    app.setApplicationVersion(APP_VERSION);

#ifdef Q_OS_LINUX
    app.setWindowIcon(QIcon(":/resources/icons/app-icon.png"));
#endif

    SettingsManager settingsManager;
    UploadManager uploadManager;
    Clipboard clipboard;

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
    parser.process(app);

    const QStringList posArgs = parser.positionalArguments();

    if (!hasCliFlags && posArgs.isEmpty()) {
        QQmlApplicationEngine engine;
        qmlRegisterSingletonType(QUrl("qrc:/qml/Theme.qml"), "QuickShare", 1, 0, "Theme");

        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
            &app, [url = QUrl(QStringLiteral("qrc:/qml/main.qml"))](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
            }, Qt::QueuedConnection);

        engine.rootContext()->setContextProperty("settingsManager", &settingsManager);
        engine.rootContext()->setContextProperty("uploadManager", &uploadManager);
        engine.rootContext()->setContextProperty("clipboard", &clipboard);
        engine.load(url);
        return app.exec();
    }

    if (!hasCliFlags && !posArgs.isEmpty()) {
        QQmlApplicationEngine engine;
        qmlRegisterSingletonType(QUrl("qrc:/qml/Theme.qml"), "QuickShare", 1, 0, "Theme");

        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
            &app, [url = QUrl(QStringLiteral("qrc:/qml/main.qml")), &posArgs, &settingsManager, &uploadManager](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl) {
                    QCoreApplication::exit(-1);
                } else if (obj && url == objUrl) {
                    obj->setProperty("visible", false);
                    QString fileName = posArgs.size() > 1 ? "Multiple files..." : QFileInfo(posArgs.first()).fileName();
                    QMetaObject::invokeMethod(obj, "showUploadPopup", 
                                            Q_ARG(QVariant, fileName), 
                                            Q_ARG(QVariant, "Obliczanie..."));
                    uploadManager.setApiKey(settingsManager.apiKey());
                    uploadManager.uploadFiles(posArgs);
                }
            }, Qt::QueuedConnection);

        engine.rootContext()->setContextProperty("settingsManager", &settingsManager);
        engine.rootContext()->setContextProperty("uploadManager", &uploadManager);
        engine.rootContext()->setContextProperty("clipboard", &clipboard);
        engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
        return app.exec();
    }

    printLimitsWarning();

    if (!posArgs.isEmpty() && posArgs[0] == "config") {
        if (posArgs.size() < 3) {
            cerr << "Usage: qs config <option> <value>" << endl;
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
        }
        return 0;
    }

    QString apiKey = parser.isSet(apiOption) ? parser.value(apiOption) : settingsManager.apiKey();
    if (apiKey.isEmpty()) {
        cerr << "Error: Pixeldrain API key is not set. Use 'qs config api <key>'" << endl;
        return 1;
    }

    if (parser.isSet(compressOption) && parser.isSet(soloOption)) {
        cerr << "Error: Conflict in flags. Cannot use both -c and -s." << endl;
        return 1;
    }

    QStringList pathsToUpload;
    if (parser.isSet(fileOption)) pathsToUpload.append(parser.value(fileOption));
    for (const QString& arg : posArgs) pathsToUpload.append(arg);

    bool compress = parser.isSet(compressOption);
    bool solo = parser.isSet(soloOption);

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
        connectLog(QFileInfo(file).fileName());
        QObject::connect(&uploader, &PixeldrainUploader::uploadFinished, [&](const QString &url) {
            cout << "\n=============================================\nLink to your file:\n" << url.toStdString() << "\n=============================================" << endl;
            loop.quit();
        });
        uploader.uploadFile(file);
        loop.exec();
    } else if (solo) {
        QString reportName = "qs-upload-data-" + QDateTime::currentDateTime().toString("dd-MM-yyyy-hh-mm-ss") + ".txt";
        QFile report(reportName); report.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&report);

        for (const QString& p : pathsToUpload) {
            uploader.uploadFile(p); 
        }
        cout << "Uploads finished. Report: " << reportName.toStdString() << endl;
    } else {
        uploader.uploadFile(pathsToUpload.first());
    }

    return 0;
}
