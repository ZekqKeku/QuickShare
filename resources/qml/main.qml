import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QuickShare 1.0

Window {
    id: mainWindow
    
    visible: true
    width: 520
    height: 580
    minimumWidth: 420
    minimumHeight: 480
    
    flags: Qt.Window | Qt.FramelessWindowHint
    color: "transparent"
    
    title: (settingsManager.translationContext, settingsManager.qsTr("app_title"))

    palette {
        window: Theme.background
        windowText: Theme.foreground
        base: Theme.input
        text: Theme.foreground
        button: Theme.card
        buttonText: Theme.foreground
        highlight: Theme.primary
        highlightedText: Theme.primaryForeground
        accent: Theme.primary
        mid: Theme.border
        brightText: Theme.primaryForeground
        toolTipBase: Theme.card
        toolTipText: Theme.foreground
    }
    
    Connections {
        target: settingsManager
        function onThemeModeChanged() { updateTheme() }
        function onAccentColorChanged() { updateTheme() }
        function onSettingsLoaded() { updateTheme() }
        function onSystemThemeChanged() { 
            if (settingsManager.themeMode === 0) updateTheme() 
        }
    }
    
    function updateTheme() {
        var mode = settingsManager.themeMode
        if (mode === 0) { 
            Theme.isDarkMode = settingsManager.isSystemDarkMode
        } else {
            Theme.isDarkMode = (mode === 2)
        }
        Theme.accentColor = settingsManager.accentColor
    }

    Component.onCompleted: {
        updateTheme()
    }
    
    MainWindow {
        anchors.fill: parent
        anchors.margins: 8
    }
    
    Component {
        id: uploadPopupComponent
        UploadPopup {
            onCopyClicked: { clipboard.setText(fileUrl) }
            onCancelClicked: { close() }
            onCloseRequested: { close(); destroy() }
        }
    }
    
    function showUploadPopup(initialFileName, initialFileSize) {
        var popup = uploadPopupComponent.createObject(null, {
            fileName: initialFileName,
            fileSize: initialFileSize,
            x: Screen.width - 400,
            y: Screen.height - 250,
            uploadProgress: Qt.binding(function() { return uploadManager.progress }),
            uploadSpeed: Qt.binding(function() { return uploadManager.speed }),
            fileUrl: Qt.binding(function() { return uploadManager.lastUploadUrl })
        })

        popup.fileName = Qt.binding(function() { return uploadManager.currentFileName || initialFileName })
        popup.fileSize = Qt.binding(function() { return uploadManager.currentFileSize || initialFileSize })
        
        var onCompleted = function(finishedFileName, url) {
            popup.isCompleted = true
            uploadManager.uploadCompleted.disconnect(onCompleted)
        }
        uploadManager.uploadCompleted.connect(onCompleted)
        
        popup.show()
        return popup
    }
}
