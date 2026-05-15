#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFileInfo>
#include <QStringList>
#include <QDebug>
#include "cli/CliHandler.h"
#include "core/SettingsManager.h"
#include "gui/UploadController.h"
#include "utils/Clipboard.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName("QuickShare");

    QStringList rawArgs = app.arguments();
    QStringList filteredArgs;
    QStringList filesToUpload;
    bool hasCliFlags = false;

    QStringList junkPrefixes = { "-psn", "--sm-client-id" };
    QStringList junkExact = { "-reopen" };

    filteredArgs.append(rawArgs.at(0));

    QStringList cliCommands = { "config", "upload" };
    QStringList cliFlags = { "-f", "--file", "-c", "--compress", "-s", "--solo", "-a", "--api", "-h", "--help", "-v", "--version" };

    for (int i = 1; i < rawArgs.size(); ++i) {
        QString arg = rawArgs.at(i);
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

    qmlRegisterSingletonType(QUrl("qrc:/qml/Theme.qml"), "QuickShare", 1, 0, "Theme");

    SettingsManager settingsManager;
    UploadController uploadController;
    Clipboard clipboard;

    QQmlApplicationEngine engine;
    
    engine.rootContext()->setContextProperty("settingsManager", &settingsManager);
    engine.rootContext()->setContextProperty("uploadManager", &uploadController);
    engine.rootContext()->setContextProperty("clipboard", &clipboard);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, 
        [url, filesToUpload, &settingsManager, &uploadController](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                QCoreApplication::exit(-1);
            } else if (obj && url == objUrl && !filesToUpload.isEmpty()) {
                QString fileName = filesToUpload.size() > 1 ? "Wiele plików..." : QFileInfo(filesToUpload.first()).fileName();
                QMetaObject::invokeMethod(obj, "showUploadPopup", Q_ARG(QVariant, fileName), Q_ARG(QVariant, "Obliczanie..."));
                uploadController.setApiKey(settingsManager.apiKey());
                uploadController.uploadFiles(filesToUpload);
            }
        }, Qt::QueuedConnection);

    engine.load(url);
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
