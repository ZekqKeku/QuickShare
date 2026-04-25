#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>
#include <QSettings>

class AppConfig {
public:
    static AppConfig& instance();

    void setApiKey(const QString& key);
    QString apiKey() const;

    void setPremiumMode(bool enabled);
    bool isPremiumMode() const;

    void setHideInDock(bool enabled);
    bool isHideInDock() const;

    void setAutoStart(bool enabled);
    bool isAutoStart() const;

private:
    AppConfig();
    QSettings settings;
};

#endif
