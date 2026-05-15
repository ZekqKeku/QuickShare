import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QuickShare 1.0

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
    
    property string fileName: ""
    property string fileSize: ""
    property real uploadProgress: 0.0
    property string uploadSpeed: ""
    property bool isCompleted: false
    property string fileUrl: ""
    
    readonly property bool isMac: Qt.platform.os === "osx" || Qt.platform.os === "macos"
    
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
                    anchors.leftMargin: root.isMac ? 12 : Theme.spacingMedium
                    anchors.rightMargin: root.isMac ? 12 : Theme.spacingSmall
                    spacing: root.isMac ? 0 : Theme.spacingSmall

                    Rectangle {
                        visible: root.isMac
                        width: 12; height: 12; radius: 6
                        color: closeBtnAreaMac.containsMouse ? "#ff5f56" : "#ff605c"
                        border.width: 1
                        border.color: Qt.darker(color, 1.2)
                        
                        Text {
                            anchors.centerIn: parent
                            text: "\u2715"
                            font.pixelSize: 8
                            visible: closeBtnAreaMac.containsMouse
                            color: Qt.darker("#ff605c", 2.0)
                        }
                        
                        MouseArea {
                            id: closeBtnAreaMac
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: root.closeRequested()
                        }
                    }

                    Rectangle {
                        visible: !root.isMac
                        width: 24; height: 24; radius: 12
                        color: Theme.primary
                        Text { anchors.centerIn: parent; text: "Q"; color: "#ffffff"; font.pixelSize: 12; font.weight: Font.Bold }
                    }

                    Text {
                        Layout.fillWidth: true
                        text: root.isCompleted 
                            ? (settingsManager.translationContext, settingsManager.qsTr("upload_finished"))
                            : (settingsManager.translationContext, settingsManager.qsTr("quick_upload"))
                        color: Theme.foreground
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontSizeSmall
                        font.weight: Font.Medium
                        horizontalAlignment: root.isMac ? Text.AlignHCenter : Text.AlignLeft
                        
                        leftPadding: root.isMac ? -12 : 0 
                    }

                    Rectangle {
                        visible: !root.isMac
                        width: 28; height: 24; radius: Theme.radiusSmall
                        color: closeBtnArea.containsMouse ? Theme.error : "transparent"
                        Text { anchors.centerIn: parent; text: "\u2715"; color: closeBtnArea.containsMouse ? "#ffffff" : Theme.mutedForeground; font.pixelSize: 10 }
                        MouseArea { id: closeBtnArea; anchors.fill: parent; hoverEnabled: true; onClicked: root.closeRequested() }
                    }

                    Item {
                        visible: root.isMac
                        width: 12
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    anchors.leftMargin: root.isMac ? 40 : 0
                    anchors.rightMargin: root.isMac ? 0 : 40
                    property point clickPos
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onPressed: (mouse) => {
                        if (mouse.button === Qt.RightButton) {
                            root.closeRequested()
                            return
                        }
                        clickPos = Qt.point(mouse.x, mouse.y)
                    }
                    onPositionChanged: (mouse) => {
                        if (pressed && mouse.button === Qt.LeftButton) {
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
                                text: root.isCompleted ? (settingsManager.translationContext, settingsManager.qsTr("upload_finished")) : root.fileSize + " \u2022 " + root.uploadSpeed
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
                                color: Theme.input
                                border.width: 1
                                border.color: Theme.border
                                
                                TextInput {
                                    id: popupUrlInput
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
                                    selectionColor: Theme.primary
                                    selectedTextColor: "#ffffff"
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
                                    Text { text: copyBtn.copied ? (settingsManager.translationContext, settingsManager.qsTr("done")) : (settingsManager.translationContext, settingsManager.qsTr("copy")); color: "#ffffff"; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSizeSmall; font.weight: Font.Medium }
                                }
                                
                                MouseArea {
                                    id: copyBtnArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        root.copyClicked()
                                        copyBtn.copied = true
                                        popupUrlInput.selectAll()
                                        autoCloseTimer.start()
                                    }
                                }
                            }
                        }
                        
                        Text {
                            text: (settingsManager.translationContext, settingsManager.qsTr("link_copied"))
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
