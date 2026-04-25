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
