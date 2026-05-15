// WindowFrame.qml - Ramka okna w stylu desktop
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QuickShare 1.0

Rectangle {
    id: root
    
    property string title: "QuickShare"
    property string subtitle: ""
    property bool showMinimize: true
    property bool showMaximize: true
    property bool showClose: true
    property alias content: contentArea.data
    
    signal minimizeClicked()
    signal maximizeClicked()
    signal closeClicked()
    
    color: Theme.background
    border.width: 1
    border.color: Theme.border
    radius: Theme.radiusMedium
    
    // Pasek tytulowy
    Rectangle {
        id: titleBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 1
        height: 40
        color: Theme.card
        radius: Theme.radiusMedium
        
        // Zaokraglenie tylko na gorze
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: parent.radius
            color: parent.color
        }
        
        // Obszar do przeciagania okna
        MouseArea {
            anchors.fill: parent
            anchors.rightMargin: windowButtons.width + 10
            property point clickPos
            
            onPressed: clickPos = Qt.point(mouse.x, mouse.y)
            onPositionChanged: {
                if (pressed) {
                    var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                    Window.window.x += delta.x
                    Window.window.y += delta.y
                }
            }
        }
        
        // Ikona i tytul
        RowLayout {
            anchors.left: parent.left
            anchors.leftMargin: Theme.spacingMedium
            anchors.verticalCenter: parent.verticalCenter
            spacing: Theme.spacingSmall
            
            // Ikona aplikacji
            Rectangle {
                width: 20
                height: 20
                radius: 4
                color: Theme.primary
                
                Text {
                    anchors.centerIn: parent
                    text: "Q"
                    color: Theme.primaryForeground
                    font.pixelSize: 12
                    font.weight: Font.Bold
                    font.family: Theme.fontFamily
                }
            }
            
            Text {
                text: root.title
                color: Theme.foreground
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeNormal
                font.weight: Font.Medium
            }
            
            Text {
                visible: root.subtitle !== ""
                text: "- " + root.subtitle
                color: Theme.mutedForeground
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeSmall
            }
        }
        
        // Przyciski okna
        Row {
            id: windowButtons
            anchors.right: parent.right
            anchors.rightMargin: Theme.spacingSmall
            anchors.verticalCenter: parent.verticalCenter
            spacing: 2
            
            // Minimize
            Rectangle {
                visible: root.showMinimize
                width: 32
                height: 28
                radius: Theme.radiusSmall
                color: minimizeArea.containsMouse ? Theme.secondary : "transparent"
                
                Rectangle {
                    anchors.centerIn: parent
                    width: 10
                    height: 2
                    radius: 1
                    color: Theme.mutedForeground
                }
                
                MouseArea {
                    id: minimizeArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.minimizeClicked()
                }
            }
            
            // Maximize
            Rectangle {
                visible: root.showMaximize
                width: 32
                height: 28
                radius: Theme.radiusSmall
                color: maximizeArea.containsMouse ? Theme.secondary : "transparent"
                
                Rectangle {
                    anchors.centerIn: parent
                    width: 10
                    height: 10
                    radius: 2
                    color: "transparent"
                    border.width: 2
                    border.color: Theme.mutedForeground
                }
                
                MouseArea {
                    id: maximizeArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.maximizeClicked()
                }
            }
            
            // Close
            Rectangle {
                visible: root.showClose
                width: 32
                height: 28
                radius: Theme.radiusSmall
                color: closeArea.containsMouse ? Theme.destructive : "transparent"
                
                Text {
                    anchors.centerIn: parent
                    text: "\u2715"
                    color: closeArea.containsMouse ? Theme.destructiveForeground : Theme.mutedForeground
                    font.pixelSize: 12
                }
                
                MouseArea {
                    id: closeArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.closeClicked()
                }
            }
        }
    }
    
    // Obszar zawartosci
    Item {
        id: contentArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: titleBar.bottom
        anchors.bottom: statusBar.top
        anchors.margins: 1
    }
    
    // Pasek statusu
    Rectangle {
        id: statusBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 1
        height: 28
        color: Theme.card
        radius: Theme.radiusMedium
        
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: parent.radius
            color: parent.color
        }
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Theme.spacingMedium
            anchors.rightMargin: Theme.spacingMedium
            
            Text {
                text: "QuickShare v1.0.0"
                color: Theme.mutedForeground
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeSmall
            }
            
            Item { Layout.fillWidth: true }
            
            // Status polaczenia
            Row {
                spacing: Theme.spacingTiny
                
                Rectangle {
                    width: 8
                    height: 8
                    radius: 4
                    color: Theme.success
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                Text {
                    text: "Polaczono z Pixeldrain"
                    color: Theme.mutedForeground
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeSmall
                }
            }
        }
    }
}
