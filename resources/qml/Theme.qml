pragma Singleton
import QtQuick

QtObject {
    id: theme
    
    property bool isDarkMode: true
    property color accentColor: "#00A5DA"
    
    property var palettes: settingsManager.palettes
    
    readonly property var accentColors: palettes.accentColors || [
        { "dark": "#00A5DA", "light": "#007194" },
        { "dark": "#22C55E", "light": "#15803d" },
        { "dark": "#EF4444", "light": "#b91c1c" },
        { "dark": "#F59E0B", "light": "#b45309" },
        { "dark": "#8B5CF6", "light": "#6d28d9" }
    ]

    function getActiveAccent(baseColor, dark) {
        var colors = accentColors
        if (!colors) return baseColor;
        
        var baseStr = String(baseColor).toUpperCase();
        for (var i = 0; i < colors.length; i++) {
            var darkStr = String(colors[i].dark).toUpperCase();
            var lightStr = String(colors[i].light).toUpperCase();
            
            if (darkStr === baseStr || lightStr === baseStr) {
                return dark ? colors[i].dark : colors[i].light;
            }
        }
        return baseColor;
    }

    property var currentPalette: (palettes && palettes[isDarkMode ? "dark" : "light"]) || {}

    property color background: currentPalette.background || (isDarkMode ? "#09090b" : "#ffffff")
    property color foreground: currentPalette.foreground || (isDarkMode ? "#fafafa" : "#09090b")
    property color card: currentPalette.card || (isDarkMode ? "#18181b" : "#f4f4f5")
    property color cardHover: currentPalette.cardHover || (isDarkMode ? "#27272a" : "#e4e4e7")
    property color border: currentPalette.border || (isDarkMode ? "#27272a" : "#e4e4e7")
    
    property color primary: getActiveAccent(accentColor, isDarkMode)
    property color primaryHover: Qt.lighter(primary, 1.1)
    property color primaryForeground: "#ffffff"
    
    property color secondary: currentPalette.muted || (isDarkMode ? "#27272a" : "#f4f4f5")
    property color secondaryHover: Qt.lighter(secondary, 1.05)
    property color mutedForeground: currentPalette.mutedForeground || (isDarkMode ? "#a1a1aa" : "#71717a")
    
    property color input: currentPalette.input || (isDarkMode ? "#18181b" : "#ffffff")
    property color inputBorder: currentPalette.inputBorder || (isDarkMode ? "#27272a" : "#e4e4e7")
    
    property color success: "#22c55e"
    property color error: "#ef4444"
    property color destructive: "#ef4444"

    readonly property string fontFamily: "sans-serif"
    readonly property int fontSizeNormal: 13
    readonly property int fontSizeMedium: 14
    readonly property int fontSizeLarge: 16
    readonly property int fontSizeSmall: 11
    
    readonly property real radiusNormal: 6
    readonly property real radiusLarge: 12
    readonly property real radiusMedium: 8
    readonly property real radiusSmall: 4
    
    readonly property real spacingSmall: 8
    readonly property real spacingMedium: 16
    readonly property real spacingLarge: 24
    readonly property real spacingNormal: 12
    
    readonly property int animationNormal: 200
}
