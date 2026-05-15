pragma Singleton
import QtQuick 2.15

QtObject {
    id: theme
    
    property bool isDarkMode: true
    property color accentColor: "#00A5DA"
    
    readonly property color background: isDarkMode ? "#121214" : "#f8f8fc"
    readonly property color foreground: isDarkMode ? "#f0f0f5" : "#1a1a1f"
    readonly property color card: isDarkMode ? "#1a1a1c" : "#ffffff"
    readonly property color cardHover: isDarkMode ? "#222225" : "#f5f5f8"
    readonly property color border: isDarkMode ? "#2a2a2e" : "#e0e0e5"
    readonly property color borderLight: isDarkMode ? "#3a3a40" : "#d0d0d8"
    
    readonly property color primary: accentColor
    readonly property color primaryHover: Qt.lighter(accentColor, 1.1)
    readonly property color primaryForeground: "#ffffff"
    
    readonly property color secondary: isDarkMode ? "#242426" : "#f0f0f5"
    readonly property color secondaryHover: isDarkMode ? "#2d2d30" : "#e5e5ea"
    readonly property color secondaryForeground: isDarkMode ? "#a0a0a8" : "#505058"
    
    readonly property color accent: accentColor
    readonly property color accentForeground: "#ffffff"
    
    readonly property color muted: isDarkMode ? "#242426" : "#f0f0f5"
    readonly property color mutedForeground: isDarkMode ? "#70707c" : "#606068"
    
    readonly property color destructive: "#ef4444"
    readonly property color destructiveForeground: "#ffffff"
    
    readonly property color success: "#22c55e"
    readonly property color warning: "#f59e0b"
    readonly property color error: "#ef4444"
    
    readonly property color input: isDarkMode ? "#1a1a1c" : "#ffffff"
    readonly property color inputBorder: isDarkMode ? "#2a2a2e" : "#d0d0d8"
    
    readonly property string fontFamily: "Segoe UI, SF Pro Display, Ubuntu, sans-serif"
    readonly property int fontSizeSmall: 11
    readonly property int fontSizeNormal: 13
    readonly property int fontSizeMedium: 14
    readonly property int fontSizeLarge: 16
    readonly property int fontSizeTitle: 18
    
    readonly property int radiusSmall: 4
    readonly property int radiusNormal: 6
    readonly property int radiusMedium: 8
    readonly property int radiusLarge: 12
    
    readonly property int spacingTiny: 4
    readonly property int spacingSmall: 8
    readonly property int spacingNormal: 12
    readonly property int spacingMedium: 16
    readonly property int spacingLarge: 24
    
    readonly property int borderWidth: 1
    
    readonly property int animationFast: 150
    readonly property int animationNormal: 250
    readonly property int animationSlow: 400
}
