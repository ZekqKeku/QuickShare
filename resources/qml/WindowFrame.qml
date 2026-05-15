import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QuickShare 1.0

Rectangle {
    id: root
    
    property string title: "QuickShare"
    property string subtitle: ""
    property bool showMinimize: true
    property bool showMaximize: true
    property bool showClose: true
    property alias content: contentArea.data
    
    readonly property bool isMac: Qt.platform.os === "osx" || Qt.platform.os === "macos"
    
    signal minimizeClicked()
    signal maximizeClicked()
    signal closeClicked()
    
    color: Theme.background
    border.width: 1
    border.color: Theme.border
    radius: Theme.radiusMedium
    
    Rectangle {
        id: titleBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 1
        height: 40
        color: Theme.card
        radius: Theme.radiusMedium
        
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: parent.radius
            color: parent.color
        }
        
        MouseArea {
            anchors.fill: parent
            anchors.leftMargin: root.isMac ? 80 : 0
            anchors.rightMargin: root.isMac ? 0 : windowButtons.width + 10
            property point clickPos
            
            onPressed: (mouse) => clickPos = Qt.point(mouse.x, mouse.y)
            onPositionChanged: (mouse) => {
                if (pressed) {
                    var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                    Window.window.x += delta.x
                    Window.window.y += delta.y
                }
            }
        }
        
        Row {
            id: macButtons
            visible: root.isMac
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            spacing: 8
            
            Rectangle {
                visible: root.showClose
                width: 12; height: 12; radius: 6
                color: closeAreaMac.containsMouse ? "#ff5f56" : "#ff605c"
                border.width: 1
                border.color: Qt.darker(color, 1.2)
                
                Text {
                    anchors.centerIn: parent
                    text: "\u2715"
                    font.pixelSize: 8
                    visible: closeAreaMac.containsMouse
                    color: Qt.darker("#ff605c", 2.0)
                }
                
                MouseArea {
                    id: closeAreaMac
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.closeClicked()
                }
            }
            
            Rectangle {
                visible: root.showMinimize
                width: 12; height: 12; radius: 6
                color: minimizeAreaMac.containsMouse ? "#ffbd2e" : "#ffbd2e"
                border.width: 1
                border.color: Qt.darker(color, 1.2)
                
                Rectangle {
                    anchors.centerIn: parent
                    width: 6; height: 1
                    visible: minimizeAreaMac.containsMouse
                    color: Qt.darker("#ffbd2e", 2.0)
                }
                
                MouseArea {
                    id: minimizeAreaMac
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.minimizeClicked()
                }
            }
            
            Rectangle {
                visible: root.showMaximize
                width: 12; height: 12; radius: 6
                color: maximizeAreaMac.containsMouse ? "#27c93f" : "#28c840"
                border.width: 1
                border.color: Qt.darker(color, 1.2)
                
                Text {
                    anchors.centerIn: parent
                    text: "\u2922"
                    font.pixelSize: 8
                    visible: maximizeAreaMac.containsMouse
                    color: Qt.darker("#28c840", 2.0)
                }
                
                MouseArea {
                    id: maximizeAreaMac
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.maximizeClicked()
                }
            }
        }
        
        RowLayout {
            anchors.centerIn: parent
            visible: root.isMac
            spacing: Theme.spacingSmall
            
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
        
        RowLayout {
            anchors.left: parent.left
            anchors.leftMargin: Theme.spacingMedium
            anchors.verticalCenter: parent.verticalCenter
            visible: !root.isMac
            spacing: Theme.spacingSmall
            
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
        
        Row {
            id: windowButtons
            visible: !root.isMac
            anchors.right: parent.right
            anchors.rightMargin: Theme.spacingSmall
            anchors.verticalCenter: parent.verticalCenter
            spacing: 2
            
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
    
    Item {
        id: contentArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: titleBar.bottom
        anchors.bottom: statusBar.top
        anchors.margins: 1
    }
    
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
                text: "QuickShare v" + Qt.application.version
                color: Theme.mutedForeground
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeSmall
            }
            
            Item { Layout.fillWidth: true }
            
            Row {
                spacing: Theme.spacingSmall
                
                Rectangle {
                    width: 8
                    height: 8
                    radius: 4
                    color: uploadManager.isApiValid ? Theme.success : Theme.error
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                Text {
                    text: uploadManager.isApiValid 
                        ? (settingsManager.translationContext, settingsManager.qsTr("status_connected"))
                        : (settingsManager.translationContext, settingsManager.qsTr("status_error"))
                    color: Theme.mutedForeground
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeSmall
                }
            }
        }
    }
}
