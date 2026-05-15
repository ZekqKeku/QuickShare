#include "SystemUtils.h"
#include <QCoreApplication>
#include <QSettings>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include "../core/AppConfig.h"

#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

void SystemUtils::setAutoStart(bool enabled) {
    QString appName = "QuickShare";
    QString appPath = QCoreApplication::applicationFilePath();

#ifdef Q_OS_MAC
    if (appPath.contains(".app/Contents/MacOS/")) {
        appPath = appPath.left(appPath.lastIndexOf(".app/Contents/MacOS/") + 4);
    }
#endif

#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if (enabled) {
        settings.setValue(appName, QDir::toNativeSeparators(appPath));
    } else {
        settings.remove(appName);
    }
#elif defined(Q_OS_MAC)
    QString cmd;
    if (enabled) {
        cmd = QString("osascript -e 'tell application \"System Events\" to make login item at end with properties {path:\"%1\", hidden:false, name:\"%2\"}'").arg(appPath, appName);
    } else {
        cmd = QString("osascript -e 'tell application \"System Events\" to delete login item \"%1\"'").arg(appName);
    }
    
    QProcess process;
    process.start("bash", {"-c", cmd});
    process.waitForFinished();
#elif defined(Q_OS_LINUX)
    QString autostartDir = QDir::homePath() + "/.config/autostart";
    QDir().mkpath(autostartDir);
    QString desktopFile = autostartDir + "/" + appName + ".desktop";

    if (enabled) {
        QFile file(desktopFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "[Desktop Entry]\n";
            out << "Type=Application\n";
            out << "Exec=" << appPath << "\n";
            out << "Hidden=false\n";
            out << "NoDisplay=false\n";
            out << "X-GNOME-Autostart-enabled=true\n";
            out << "Name=" << appName << "\n";
            out << "Comment=Start QuickShare on login\n";
        }
    } else {
        QFile::remove(desktopFile);
    }
#endif
}

void SystemUtils::setHideInDock(bool enabled) {
}

void SystemUtils::applySystemSettings() {
    setAutoStart(AppConfig::instance().isAutoStart());
    registerContextMenu(true);
}

void SystemUtils::registerContextMenu(bool enabled) {
    QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    QString appName = "QuickShare";

#ifdef Q_OS_WIN
    QString shellKeyFiles = "HKEY_CURRENT_USER\\Software\\Classes\\*\\shell\\" + appName;
    QString shellKeyDirs = "HKEY_CURRENT_USER\\Software\\Classes\\Directory\\shell\\" + appName;

    QSettings regFiles(shellKeyFiles, QSettings::NativeFormat);
    QSettings regDirs(shellKeyDirs, QSettings::NativeFormat);

    if (enabled) {
        regFiles.setValue(".", "Share via QuickShare");
        regFiles.setValue("command/.", "\"" + appPath + "\" \"%1\"");
        regDirs.setValue(".", "Share via QuickShare");
        regDirs.setValue("command/.", "\"" + appPath + "\" \"%1\"");
    } else {
        regFiles.remove("");
        regDirs.remove("");
    }
#elif defined(Q_OS_MAC)
    QString destPath = QDir::homePath() + "/Library/Services/Share via QuickShare.workflow";

    if (enabled) {
        QDir().mkpath(destPath + "/Contents");
        
        QFile plist(destPath + "/Contents/Info.plist");
        if (plist.open(QIODevice::WriteOnly)) {
            QTextStream out(&plist);
            out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
                << "<plist version=\"1.0\">\n<dict>\n"
                << "  <key>NSServices</key>\n  <array>\n    <dict>\n"
                << "      <key>NSMenuItem</key>\n      <dict>\n"
                << "        <key>default</key>\n        <string>Share via QuickShare</string>\n"
                << "      </dict>\n"
                << "      <key>NSMessage</key>\n      <string>runWorkflowAsService</string>\n"
                << "      <key>NSRequiredContext</key>\n      <dict/>\n"
                << "      <key>NSSendTypes</key>\n      <array>\n"
                << "        <string>public.item</string>\n      </array>\n"
                << "    </dict>\n  </array>\n</dict>\n</plist>";
        }

        QFile wflow(destPath + "/Contents/document.wflow");
        if (wflow.open(QIODevice::WriteOnly)) {
            QTextStream out(&wflow);
            out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
                << "<plist version=\"1.0\">\n<dict>\n"
                << "  <key>actions</key>\n  <array>\n    <dict>\n"
                << "      <key>action</key>\n      <dict>\n"
                << "        <key>ActionParameters</key>\n        <dict>\n"
                << "          <key>COMMAND_STRING</key>\n"
                << "          <string>\"" << appPath << "\" \"$@\"</string>\n"
                << "          <key>inputMethod</key>\n          <integer>1</integer>\n"
                << "        </dict>\n"
                << "        <key>ActionBundlePath</key>\n        <string>/System/Library/Automator/Run Shell Script.action</string>\n"
                << "        <key>ActionName</key>\n        <string>Run Shell Script</string>\n"
                << "      </dict>\n    </dict>\n  </array>\n"
                << "  <key>workflowMetaData</key>\n  <dict>\n"
                << "    <key>serviceInputTypeIdentifier</key>\n    <string>com.apple.Automator.fileSystemObject</string>\n"
                << "    <key>workflowTypeIdentifier</key>\n    <string>com.apple.Automator.servicesMenu</string>\n"
                << "  </dict>\n</dict>\n</plist>";
        }
        QProcess::execute("killall", {"pbs"});
    } else {
        QProcess::execute("rm", {"-rf", destPath});
        QProcess::execute("killall", {"pbs"});
    }
#elif defined(Q_OS_LINUX)
    QString desktopPath = QDir::homePath() + "/.local/share/applications/com.quickshare.app.desktop";
    if (enabled) {
        QString sourcePath = QCoreApplication::applicationDirPath() + "/../share/applications/com.quickshare.app.desktop";
        if (!QFile::exists(sourcePath)) {
            sourcePath = "/usr/share/applications/com.quickshare.app.desktop";
        }
        if (QFile::exists(sourcePath)) {
            QFile::copy(sourcePath, desktopPath);
        }
    } else {
        QFile::remove(desktopPath);
    }
#endif
}
