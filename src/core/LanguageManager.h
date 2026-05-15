#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QString>
#include <QJsonObject>
#include <QMap>

class LanguageManager {
public:
    static LanguageManager& instance();
    void setLanguage(const QString& langCode);
    QString getString(const QString& key) const;
    QString currentLanguage() const;

private:
    LanguageManager();
    QJsonObject currentStrings;
    QString currentLang;
    void loadLanguage(const QString& langCode);
};

#endif
