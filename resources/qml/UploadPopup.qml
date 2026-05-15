import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
    id: root
    
    width: 400
    height: isCompleted ? 220 : 180
    minimumWidth: 400
    minimumHeight: isCompleted ? 220 : 180
    maximumWidth: 400
    maximumHeight: isCompleted ? 220 : 180
    
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"
    
    property string fileName: "plik.pdf"
    property string fileSize: "2.4 MB"
    property real uploadProgress: 0.0
    property string uploadSpeed: "0 KB/s"
    property bool isCompleted: false
    property string fileUrl: ""
    
    signal copyClicked()
    signal cancelClicked()
    signal closeRequested()
    
    Timer {
        id: autoCloseTimer
        interval: 3000
        onTriggered: root.closeRequested()
    }
    
    Rectangle {
        anchors.fill: parent
        anchors.margins: 10
        radius: Theme.radiusLarge
        color: Theme.background
        border.width: 1
        border.color: Theme.border
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            
            Rectangle {
                Layout.fillWidth: true
                height: 40
                color: Theme.card
                radius: Theme.radiusLarge
                
                Rectangle {
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: parent.radius
                    color: parent.color
                }
                
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: Theme.spacingMedium
                    anchors.rightMargin: Theme.spacingSmall
                    
                    Rectangle {
                        width: 20; height: 20; radius: 4
                        color: Theme.primary
                        Text { anchors.centerIn: parent; text: "Q"; color: "#ffffff"; font.pixelSize: 11; font.weight: Font.Bold }
                    }
                    
                    Text {
                        text: "Szybkie Przesylanie"
                        color: Theme.foreground
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontSizeSmall
                        font.weight: Font.Medium
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    Rectangle {
                        width: 28; height: 24; radius: Theme.radiusSmall
                        color: closeBtnArea.containsMouse ? Theme.destructive : "transparent"
                        Text { anchors.centerIn: parent; text: "\u2715"; color: closeBtnArea.containsMouse ? "#ffffff" : Theme.mutedForeground; font.pixelSize: 10 }
                        MouseArea { id: closeBtnArea; anchors.fill: parent; hoverEnabled: true; onClicked: root.closeRequested() }
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    anchors.rightMargin: 40
                    property point clickPos
                    onPressed: clickPos = Qt.point(mouse.x, mouse.y)
                    onPositionChanged: {
                        if (pressed) {
                            var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                            root.x += delta.x
                            root.y += delta.y
                        }
                    }
                }
            }
            
            Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }
            
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingMedium
                    spacing: Theme.spacingMedium
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingNormal
                        
                        Rectangle {
                            width: 44; height: 44; radius: Theme.radiusNormal
                            color: root.isCompleted ? Qt.rgba(Theme.success.r, Theme.success.g, Theme.success.b, 0.1) : Theme.secondary
                            Text {
                                anchors.centerIn: parent
                                text: root.isCompleted ? "\u2713" : "\uD83D\uDCC4"
                                font.pixelSize: 20
                                color: root.isCompleted ? Theme.success : Theme.primary
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            Text {
                                text: root.fileName
                                color: Theme.foreground
                                font.family: Theme.fontFamily
                                font.pixelSize: Theme.fontSizeNormal
                                font.weight: Font.Medium
                                elide: Text.ElideMiddle
                                Layout.fillWidth: true
                            }
                            Text {
                                text: root.isCompleted ? "Przesylanie zakonczone" : root.fileSize + " \u2022 " + root.uploadSpeed
                                color: root.isCompleted ? Theme.success : Theme.mutedForeground
                                font.family: Theme.fontFamily
                                font.pixelSize: Theme.fontSizeSmall
                            }
                        }
                    }
                    
                    ColumnLayout {
                        visible: !root.isCompleted
                        Layout.fillWidth: true
                        spacing: Theme.spacingSmall
                        
                        CustomProgressBar {
                            Layout.fillWidth: true
                            value: root.uploadProgress
                            barHeight: 6
                        }
                        
                        RowLayout {
                            Layout.fillWidth: true
                            Text {
                                text: Math.floor(root.uploadProgress * 100) + "%"
                                color: Theme.primary
                                font.family: Theme.fontFamily
                                font.pixelSize: Theme.fontSizeSmall
                                font.weight: Font.Bold
                            }
                            Item { Layout.fillWidth: true }
                            Text {
                                text: root.uploadSpeed
                                color: Theme.mutedForeground
                                font.family: Theme.fontFamily
                                font.pixelSize: Theme.fontSizeSmall
                            }
                        }
                    }
                    
                    ColumnLayout {
                        visible: root.isCompleted
                        Layout.fillWidth: true
                        spacing: Theme.spacingSmall
                        
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: Theme.spacingSmall
                            
                            Rectangle {
                                Layout.fillWidth: true
                                height: 36
                                radius: Theme.radiusNormal
                                color: Theme.secondary
                                border.width: 1
                                border.color: Theme.border
                                
                                TextInput {
                                    anchors.fill: parent
                                    anchors.leftMargin: Theme.spacingNormal
                                    anchors.rightMargin: Theme.spacingNormal
                                    verticalAlignment: Text.AlignVCenter
                                    text: root.fileUrl
                                    color: Theme.foreground
                                    font.family: Theme.fontFamily
                                    font.pixelSize: Theme.fontSizeSmall
                                    readOnly: true
                                    selectByMouse: true
                                }
                            }
                            
                            Rectangle {
                                id: copyBtn
                                width: 100; height: 36; radius: Theme.radiusNormal
                                color: copyBtnArea.containsMouse ? Theme.primaryHover : Theme.primary
                                
                                property bool copied: false
                                
                                RowLayout {
                                    anchors.centerIn: parent
                                    spacing: 4
                                    Text { text: copyBtn.copied ? "\u2713" : "\uD83D\uDCCB"; color: "#ffffff"; font.pixelSize: 12 }
                                    Text { text: copyBtn.copied ? "Gotowe" : "Kopiuj"; color: "#ffffff"; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSizeSmall; font.weight: Font.Medium }
                                }
                                
                                MouseArea {
                                    id: copyBtnArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        root.copyClicked()
                                        copyBtn.copied = true
                                        autoCloseTimer.start()
                                    }
                                }
                            }
                        }
                        
                        Text {
                            text: "Link skopiowano do schowka"
                            visible: copyBtn.copied
                            color: Theme.success
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeSmall
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }
            }
        }
    }
}
