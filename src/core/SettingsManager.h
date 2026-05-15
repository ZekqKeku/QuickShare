#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

class SettingsManager : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(int themeMode READ themeMode WRITE setThemeMode NOTIFY themeModeChanged)
    Q_PROPERTY(QString accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged)
    Q_PROPERTY(bool startWithSystem READ startWithSystem WRITE setStartWithSystem NOTIFY startWithSystemChanged)
    Q_PROPERTY(bool minimizeToTray READ minimizeToTray WRITE setMinimizeToTray NOTIFY minimizeToTrayChanged)
    Q_PROPERTY(QVariantList availableLanguages READ availableLanguages NOTIFY availableLanguagesChanged)
    Q_PROPERTY(QVariantMap palettes READ palettes NOTIFY palettesChanged)
    Q_PROPERTY(int translationContext READ translationContext NOTIFY languageChanged)
    Q_PROPERTY(bool isSystemDarkMode READ isSystemDarkMode NOTIFY systemThemeChanged)
    
public:
    explicit SettingsManager(QObject *parent = nullptr);
    
    Q_INVOKABLE QString qsTr(const QString &key) const;
    bool isSystemDarkMode() const;
    
    QString apiKey() const;
    QString language() const;
    int themeMode() const;
    QString accentColor() const;
    bool startWithSystem() const;
    bool minimizeToTray() const;
    QVariantList availableLanguages() const;
    QVariantMap palettes() const;
    int translationContext() const { return m_translationContext; }
    
    void setApiKey(const QString &apiKey);
    void setLanguage(const QString &language);
    void setThemeMode(int mode);
    void setAccentColor(const QString &color);
    void setStartWithSystem(bool enabled);
    void setMinimizeToTray(bool enabled);
    
public slots:
    void save();
    void resetToDefaults();
    void load();
    void removeAllData();
    
signals:
    void apiKeyChanged();
    void languageChanged();
    void themeModeChanged();
    void accentColorChanged();
    void startWithSystemChanged();
    void minimizeToTrayChanged();
    void availableLanguagesChanged();
    void palettesChanged();
    void systemThemeChanged();
    void settingsSaved();
    void settingsLoaded();
    
private:
    void setupAutostart(bool enabled);
    void loadPalettes();
    
    QSettings m_settings;
    
    QString m_apiKey;
    QString m_language;
    int m_themeMode;
    QString m_accentColor;
    bool m_startWithSystem;
    bool m_minimizeToTray;
    QVariantMap m_palettes;
    int m_translationContext = 0;
};

#endif
