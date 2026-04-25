#include "SystemUtils.h"
#include <QCoreApplication>
#include <QSettings>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include "../core/AppConfig.h"

#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

void SystemUtils::setAutoStart(bool enabled) {
    QString appName = "QuickShare";
    QString appPath = QCoreApplication::applicationFilePath();

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
    QString bundlePath = QCoreApplication::applicationDirPath() + "/../Library/Services/Share via QuickShare.workflow";
    QString destPath = QDir::homePath() + "/Library/Services/Share via QuickShare.workflow";

    if (enabled) {
        if (QFile::exists(bundlePath)) {
            QProcess::execute("cp", {"-R", bundlePath, destPath});
            QProcess::execute("killall", {"pbs"});
        }
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
