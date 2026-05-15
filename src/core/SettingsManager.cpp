#include "SettingsManager.h"
#include "LanguageManager.h"
#include "../utils/SystemUtils.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QTimer>
#include <QSettings>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
{
    loadPalettes();
    load();
    LanguageManager::instance().setLanguage(m_language);

    QTimer *themeTimer = new QTimer(this);
    connect(themeTimer, &QTimer::timeout, this, [this](){
        static bool lastWasDark = isSystemDarkMode();
        bool currentIsDark = isSystemDarkMode();
        if (currentIsDark != lastWasDark) {
            lastWasDark = currentIsDark;
            emit systemThemeChanged();
        }
    });
    themeTimer->start(3000);
}

void SettingsManager::loadPalettes() {
    QFile file(":/colors.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        m_palettes = doc.object().toVariantMap();
        emit palettesChanged();
    }
}

QVariantMap SettingsManager::palettes() const {
    return m_palettes;
}

bool SettingsManager::isSystemDarkMode() const {
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 0;
#elif defined(Q_OS_MAC)
    QProcess process;
    process.start("defaults", {"read", "-g", "AppleInterfaceStyle"});
    process.waitForFinished();
    QString output = process.readAllStandardOutput().trimmed();
    return output == "Dark";
#elif defined(Q_OS_LINUX)
    
    QProcess process;
    process.start("gsettings", {"get", "org.gnome.desktop.interface", "color-scheme"});
    process.waitForFinished();
    QString output = process.readAllStandardOutput().trimmed();
    if (output.contains("dark")) return true;

    QSettings kdeSettings(QDir::homePath() + "/.config/kdeglobals", QSettings::IniFormat);
    QString kdeColorScheme = kdeSettings.value("General/ColorScheme").toString();
    if (kdeColorScheme.contains("Dark", Qt::CaseInsensitive)) return true;

    process.start("gsettings", {"get", "org.gnome.desktop.interface", "gtk-theme"});
    process.waitForFinished();
    output = process.readAllStandardOutput().trimmed();
    if (output.contains("dark", Qt::CaseInsensitive)) return true;
    
    return false;
#else
    return true; 
#endif
}

QString SettingsManager::qsTr(const QString &key) const {
    return LanguageManager::instance().getString(key);
}

QString SettingsManager::apiKey() const { return m_apiKey; }
QString SettingsManager::language() const { return m_language; }
int SettingsManager::themeMode() const { return m_themeMode; }
QString SettingsManager::accentColor() const { return m_accentColor; }
bool SettingsManager::startWithSystem() const { return m_startWithSystem; }
bool SettingsManager::minimizeToTray() const { return m_minimizeToTray; }

QVariantList SettingsManager::availableLanguages() const {
    QVariantList langs;
    QStringList searchPaths;
    searchPaths << QCoreApplication::applicationDirPath() + "/lang";
    searchPaths << QCoreApplication::applicationDirPath() + "/../Resources/lang";
    searchPaths << QDir::currentPath() + "/src/lang";
    searchPaths << QDir::currentPath() + "/QuickShare/src/lang";

    for (const QString& path : searchPaths) {
        QDir dir(path);
        if (dir.exists()) {
            QStringList files = dir.entryList({"*.json"}, QDir::Files);
            for (const QString& file : files) {
                QString code = file.section('.', 0, 0);
                QFile f(path + "/" + file);
                if (f.open(QIODevice::ReadOnly)) {
                    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
                    QString name = doc.object().value("name").toString(code);
                    
                    bool exists = false;
                    for (const QVariant& v : langs) {
                        if (v.toMap()["code"].toString() == code) { exists = true; break; }
                    }
                    if (!exists) {
                        QVariantMap m;
                        m["code"] = code;
                        m["name"] = name;
                        langs << m;
                    }
                }
            }
        }
    }
    if (langs.isEmpty()) {
        QVariantMap m; m["code"] = "enUS"; m["name"] = "English";
        langs << m;
    }
    return langs;
}

void SettingsManager::setApiKey(const QString &apiKey) {
    if (m_apiKey != apiKey) {
        m_apiKey = apiKey;
        emit apiKeyChanged();
    }
}

void SettingsManager::setLanguage(const QString &language) {
    if (m_language != language) {
        m_language = language;
        LanguageManager::instance().setLanguage(m_language);
        m_translationContext++;
        emit languageChanged();
    }
}

void SettingsManager::setThemeMode(int mode) {
    if (m_themeMode != mode) {
        m_themeMode = mode;
        emit themeModeChanged();
    }
}

void SettingsManager::setAccentColor(const QString &color) {
    if (m_accentColor != color) {
        m_accentColor = color;
        emit accentColorChanged();
    }
}

void SettingsManager::setStartWithSystem(bool enabled) {
    if (m_startWithSystem != enabled) {
        m_startWithSystem = enabled;
        emit startWithSystemChanged();
    }
}

void SettingsManager::setMinimizeToTray(bool enabled) {
    if (m_minimizeToTray != enabled) {
        m_minimizeToTray = enabled;
        emit minimizeToTrayChanged();
    }
}

void SettingsManager::save() {
    m_settings.setValue("apiKey", m_apiKey);
    m_settings.setValue("language", m_language);
    m_settings.setValue("themeMode", m_themeMode);
    m_settings.setValue("accentColor", m_accentColor);
    m_settings.setValue("autoStart", m_startWithSystem);
    m_settings.setValue("hideInDock", m_minimizeToTray);
    
    SystemUtils::setAutoStart(m_startWithSystem);
    SystemUtils::registerContextMenu(true);
    
    emit settingsSaved();
}

void SettingsManager::resetToDefaults() {
    setApiKey("");
    setLanguage("enUS");
    setThemeMode(2); 
    setAccentColor("#00A5DA");
    setStartWithSystem(false);
    setMinimizeToTray(true);
    save();
}

void SettingsManager::load() {
    m_apiKey = m_settings.value("apiKey", "").toString();
    m_language = m_settings.value("language", "enUS").toString();
    m_themeMode = m_settings.value("themeMode", 2).toInt(); 
    m_accentColor = m_settings.value("accentColor", "#00A5DA").toString();
    m_startWithSystem = m_settings.value("autoStart", false).toBool();
    m_minimizeToTray = m_settings.value("hideInDock", true).toBool();
    
    emit settingsLoaded();
    emit apiKeyChanged();
    emit languageChanged();
    emit themeModeChanged();
    emit accentColorChanged();
    emit startWithSystemChanged();
    emit minimizeToTrayChanged();
}

void SettingsManager::removeAllData() {
    
    SystemUtils::setAutoStart(false);
    SystemUtils::registerContextMenu(false);

    m_settings.clear();
    m_settings.sync();

    resetToDefaults();

    emit settingsSaved();
}
