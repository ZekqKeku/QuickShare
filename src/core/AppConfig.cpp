#include "AppConfig.h"

AppConfig& AppConfig::instance() {
    static AppConfig inst;
    return inst;
}

AppConfig::AppConfig() : settings("QuickShare", "QuickShareApp") {}

void AppConfig::setApiKey(const QString& key) {
    settings.setValue("api_key", key);
}

QString AppConfig::apiKey() const {
    return settings.value("api_key").toString();
}

void AppConfig::setPremiumMode(bool enabled) {
    settings.setValue("premium_mode", enabled);
}

bool AppConfig::isPremiumMode() const {
    return settings.value("premium_mode", false).toBool();
}

void AppConfig::setHideInDock(bool enabled) {
    settings.setValue("hide_in_dock", enabled);
}

bool AppConfig::isHideInDock() const {
    return settings.value("hide_in_dock", false).toBool();
}

void AppConfig::setAutoStart(bool enabled) {
    settings.setValue("autostart", enabled);
}

bool AppConfig::isAutoStart() const {
    return settings.value("autostart", false).toBool();
}

void AppConfig::setLanguage(const QString& lang) {
    settings.setValue("language", lang);
}

QString AppConfig::language() const {
    return settings.value("language", "enUS").toString();
}
