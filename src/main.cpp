#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QFileInfo>
#include <QStringList>
#include <QDebug>
#include <QLocalServer>
#include <QLocalSocket>
#include <QScreen>
#include "cli/CliHandler.h"
#include "core/SettingsManager.h"
#include "gui/UploadController.h"
#include "utils/Clipboard.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQuickStyle::setStyle("Basic");
    
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName("QuickShare");

    QStringList rawArgs = app.arguments();
    QStringList filteredArgs;
    QStringList filesToUpload;
    bool hasCliFlags = false;
    bool startInBackground = false;

    QStringList junkPrefixes = { "-psn", "--sm-client-id" };
    QStringList junkExact = { "-reopen" };

    filteredArgs.append(rawArgs.at(0));

    QStringList cliCommands = { "config", "upload" };
    QStringList cliFlags = { "-f", "--file", "-c", "--compress", "-s", "--solo", "-a", "--api", "-h", "--help", "-v", "--version" };

    for (int i = 1; i < rawArgs.size(); ++i) {
        QString arg = rawArgs.at(i);
        if (arg == "--background") {
            startInBackground = true;
            continue;
        }
        
        bool isJunk = false;
        for (const QString& prefix : junkPrefixes) {
            if (arg.startsWith(prefix)) { isJunk = true; break; }
        }
        if (!isJunk) {
            for (const QString& exact : junkExact) {
                if (arg == exact) { isJunk = true; break; }
            }
        }
        if (isJunk) continue;
        
        filteredArgs.append(arg);
        
        if (cliCommands.contains(arg) || cliFlags.contains(arg)) {
            hasCliFlags = true;
        } else if (!arg.startsWith("-")) {
            filesToUpload.append(arg);
        }
    }

    if (hasCliFlags) {
        CliHandler cli;
        return cli.execute(filteredArgs);
    }

    QString serverName = "QuickShare_SingleInstance";
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500)) {
        if (!filesToUpload.isEmpty()) {
            socket.write(filesToUpload.join("|").toUtf8());
            socket.waitForBytesWritten();
        }
        return 0;
    }

    QLocalServer server;
    server.listen(serverName);

    qmlRegisterSingletonType(QUrl("qrc:/qml/Theme.qml"), "QuickShare", 1, 0, "Theme");

    SettingsManager settingsManager;
    UploadController uploadController;
    Clipboard clipboard;
    
    uploadController.setApiKey(settingsManager.apiKey());
    QObject::connect(&settingsManager, &SettingsManager::apiKeyChanged, [&settingsManager, &uploadController](){
        uploadController.setApiKey(settingsManager.apiKey());
    });

    QQmlApplicationEngine engine;
    
    engine.rootContext()->setContextProperty("settingsManager", &settingsManager);
    engine.rootContext()->setContextProperty("uploadManager", &uploadController);
    engine.rootContext()->setContextProperty("clipboard", &clipboard);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    
    auto handleFiles = [url, &settingsManager, &uploadController, &engine](const QStringList &files) {
        if (files.isEmpty()) return;
        QObject *rootObj = engine.rootObjects().first();
        if (rootObj) {
            QString fileName = files.size() > 1 ? settingsManager.qsTr("multiple_files") : QFileInfo(files.first()).fileName();
            QMetaObject::invokeMethod(rootObj, "showUploadPopup", Q_ARG(QVariant, fileName), Q_ARG(QVariant, settingsManager.qsTr("calculating")));
            uploadController.setApiKey(settingsManager.apiKey());
            uploadController.uploadFiles(files);
        }
    };

    QObject::connect(&server, &QLocalServer::newConnection, [&server, handleFiles]() {
        QLocalSocket *clientSocket = server.nextPendingConnection();
        QObject::connect(clientSocket, &QLocalSocket::readyRead, [clientSocket, handleFiles]() {
            QString data = QString::fromUtf8(clientSocket->readAll());
            handleFiles(data.split("|"));
            clientSocket->disconnectFromServer();
        });
    });

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, 
        [url, filesToUpload, startInBackground, &settingsManager, handleFiles](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                QCoreApplication::exit(-1);
            } else if (obj && url == objUrl) {
                bool shouldBeVisible = !startInBackground;
                if (startInBackground && filesToUpload.isEmpty()) {
                    shouldBeVisible = !settingsManager.minimizeToTray();
                } else if (startInBackground && !filesToUpload.isEmpty()) {
                    shouldBeVisible = false; 
                }
                
                obj->setProperty("visible", shouldBeVisible);
                if (!filesToUpload.isEmpty()) {
                    handleFiles(filesToUpload);
                }
            }
        }, Qt::QueuedConnection);

    engine.load(url);
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
