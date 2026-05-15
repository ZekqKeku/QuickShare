#include "SettingsManager.h"
#include "../utils/SystemUtils.h"
#include <QCoreApplication>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
{
    load();
}

QString SettingsManager::apiKey() const { return m_apiKey; }
QString SettingsManager::language() const { return m_language; }
bool SettingsManager::startWithSystem() const { return m_startWithSystem; }
bool SettingsManager::minimizeToTray() const { return m_minimizeToTray; }

void SettingsManager::setApiKey(const QString &apiKey) {
    if (m_apiKey != apiKey) {
        m_apiKey = apiKey;
        emit apiKeyChanged();
    }
}

void SettingsManager::setLanguage(const QString &language) {
    if (m_language != language) {
        m_language = language;
        emit languageChanged();
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
    m_settings.setValue("autoStart", m_startWithSystem);
    m_settings.setValue("hideInDock", m_minimizeToTray);
    
    SystemUtils::setAutoStart(m_startWithSystem);
    SystemUtils::registerContextMenu(true);
    
    emit settingsSaved();
}

void SettingsManager::resetToDefaults() {
    setApiKey("");
    setLanguage("enUS");
    setStartWithSystem(false);
    setMinimizeToTray(true);
    save();
}

void SettingsManager::load() {
    m_apiKey = m_settings.value("apiKey", "").toString();
    m_language = m_settings.value("language", "enUS").toString();
    m_startWithSystem = m_settings.value("autoStart", false).toBool();
    m_minimizeToTray = m_settings.value("hideInDock", true).toBool();
    
    emit settingsLoaded();
    emit apiKeyChanged();
    emit languageChanged();
    emit startWithSystemChanged();
    emit minimizeToTrayChanged();
}
