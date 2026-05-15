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
    QStringList paths;
    paths << QCoreApplication::applicationDirPath() + "/lang/" + langCode + ".json";
    paths << QCoreApplication::applicationDirPath() + "/../Resources/lang/" + langCode + ".json";
    paths << QDir::currentPath() + "/src/lang/" + langCode + ".json";
    paths << QDir::currentPath() + "/QuickShare/src/lang/" + langCode + ".json";

    QString foundPath;
    for (const QString& path : paths) {
        if (QFile::exists(path)) {
            foundPath = path;
            break;
        }
    }

    if (!foundPath.isEmpty()) {
        QFile file(foundPath);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            currentStrings = doc.object();
            currentLang = langCode;
        }
    }
}
