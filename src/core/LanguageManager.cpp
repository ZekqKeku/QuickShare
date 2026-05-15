#include "LanguageManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDir>
#include "AppConfig.h"

LanguageManager& LanguageManager::instance() {
    static LanguageManager inst;
    return inst;
}

LanguageManager::LanguageManager() {
    QString lang = AppConfig::instance().language();
    if (lang.isEmpty()) lang = "enUS";
    loadLanguage(lang);
}

void LanguageManager::setLanguage(const QString& langCode) {
    loadLanguage(langCode);
    AppConfig::instance().setLanguage(langCode);
}

QString LanguageManager::getString(const QString& key) const {
    return currentStrings.value(key).toString(key);
}

QString LanguageManager::currentLanguage() const {
    return currentLang;
}

void LanguageManager::loadLanguage(const QString& langCode) {
    QString path = QCoreApplication::applicationDirPath() + "/lang/" + langCode + ".json";
    if (!QFile::exists(path)) {
        path = QDir::currentPath() + "/src/lang/" + langCode + ".json";
        if (!QFile::exists(path)) {
            path = QDir::currentPath() + "/QuickShare/src/lang/" + langCode + ".json";
        }
    }

    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        currentStrings = doc.object();
        currentLang = langCode;
    }
}
