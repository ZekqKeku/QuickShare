#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

class SettingsManager : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(bool startWithSystem READ startWithSystem WRITE setStartWithSystem NOTIFY startWithSystemChanged)
    Q_PROPERTY(bool minimizeToTray READ minimizeToTray WRITE setMinimizeToTray NOTIFY minimizeToTrayChanged)
    
public:
    explicit SettingsManager(QObject *parent = nullptr);
    
    QString apiKey() const;
    QString language() const;
    bool startWithSystem() const;
    bool minimizeToTray() const;
    
    void setApiKey(const QString &apiKey);
    void setLanguage(const QString &language);
    void setStartWithSystem(bool enabled);
    void setMinimizeToTray(bool enabled);
    
public slots:
    void save();
    void resetToDefaults();
    void load();
    
signals:
    void apiKeyChanged();
    void languageChanged();
    void startWithSystemChanged();
    void minimizeToTrayChanged();
    void settingsSaved();
    void settingsLoaded();
    
private:
    void setupAutostart(bool enabled);
    
    QSettings m_settings;
    
    QString m_apiKey;
    QString m_language;
    bool m_startWithSystem;
    bool m_minimizeToTray;
};

#endif // SETTINGSMANAGER_H
