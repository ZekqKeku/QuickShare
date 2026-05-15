import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickShare 1.0

Item {
    id: root
    
    property string apiKey: ""
    property string language: "pl"
    property bool startWithSystem: false
    property bool minimizeToTray: true
    
    signal settingsSaved()
    signal settingsReset()
    signal removeAllDataRequested()
    
    Rectangle {
        id: confirmDialog
        visible: false
        anchors.fill: parent
        color: "#AA000000"
        z: 100
        
        MouseArea {
            anchors.fill: parent
            onClicked: confirmDialog.visible = false
        }
        
        Rectangle {
            anchors.centerIn: parent
            width: Math.min(parent.width - 40, 400)
            height: dialogContent.implicitHeight + Theme.spacingLarge * 2
            radius: Theme.radiusLarge
            color: Theme.card
            border.width: 1
            border.color: Theme.border
            
            MouseArea {
                anchors.fill: parent
            }
            
            ColumnLayout {
                id: dialogContent
                anchors.fill: parent
                anchors.margins: Theme.spacingLarge
                spacing: Theme.spacingMedium
                
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 56
                    height: 56
                    radius: 28
                    color: Theme.destructive
                    opacity: 0.15
                    
                    Text {
                        anchors.centerIn: parent
                        text: "\u26A0"
                        font.pixelSize: 28
                        color: Theme.destructive
                    }
                }
                
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Potwierdz usuniecie danych"
                    color: Theme.foreground
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeLarge
                    font.weight: Font.Bold
                }
                
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    text: "Czy na pewno chcesz usunac wszystkie dane aplikacji?"
                    color: Theme.mutedForeground
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeNormal
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    Layout.topMargin: Theme.spacingNormal
                    spacing: Theme.spacingNormal
                    
                    Rectangle {
                        Layout.fillWidth: true
                        height: 40
                        radius: Theme.radiusNormal
                        color: cancelDialogBtn.containsMouse ? Theme.secondaryHover : Theme.secondary
                        border.width: 1
                        border.color: Theme.border
                        
                        Text {
                            anchors.centerIn: parent
                            text: "Anuluj"
                            color: Theme.foreground
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeNormal
                        }
                        
                        MouseArea {
                            id: cancelDialogBtn
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: confirmDialog.visible = false
                        }
                    }
                    
                    Rectangle {
                        Layout.fillWidth: true
                        height: 40
                        radius: Theme.radiusNormal
                        color: confirmDeleteBtn.containsMouse ? Qt.darker(Theme.destructive, 1.1) : Theme.destructive
                        
                        Text {
                            anchors.centerIn: parent
                            text: "Usun dane"
                            color: Theme.destructiveForeground
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeNormal
                            font.weight: Font.Medium
                        }
                        
                        MouseArea {
                            id: confirmDeleteBtn
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                confirmDialog.visible = false
                                root.removeAllDataRequested()
                            }
                        }
                    }
                }
            }
        }
    }
    
    ScrollView {
        anchors.fill: parent
        anchors.margins: Theme.spacingMedium
        clip: true
        
        ColumnLayout {
            width: parent.width
            spacing: Theme.spacingLarge
            
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: apiSection.implicitHeight + Theme.spacingMedium * 2
                radius: Theme.radiusNormal
                color: Theme.card
                border.width: 1
                border.color: Theme.border
                
                ColumnLayout {
                    id: apiSection
                    anchors.fill: parent
                    anchors.margins: Theme.spacingMedium
                    spacing: Theme.spacingNormal
                    
                    RowLayout {
                        spacing: Theme.spacingSmall
                        
                        Text {
                            text: "\uD83D\uDD11"
                            font.pixelSize: 18
                            color: Theme.primary
                        }
                        
                        ColumnLayout {
                            spacing: 2
                            
                            Text {
                                text: "Klucz API Pixeldrain"
                                color: Theme.foreground
                                font.family: Theme.fontFamily
                                font.pixelSize: Theme.fontSizeMedium
                                font.weight: Font.Medium
                            }
                            
                            Text {
                                text: "Wymagany do przesylania plikow"
                                color: Theme.mutedForeground
                                font.family: Theme.fontFamily
                                font.pixelSize: Theme.fontSizeSmall
                            }
                        }
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingSmall
                        
                        Rectangle {
                            Layout.fillWidth: true
                            height: 44
                            radius: Theme.radiusNormal
                            color: Theme.secondary
                            border.width: 1
                            border.color: apiInput.activeFocus ? Theme.primary : Theme.border
                            
                            TextInput {
                                id: apiInput
                                anchors.fill: parent
                                anchors.leftMargin: Theme.spacingNormal
                                anchors.rightMargin: Theme.spacingNormal
                                verticalAlignment: Text.AlignVCenter
                                text: root.apiKey
                                echoMode: showApiKey.checked ? TextInput.Normal : TextInput.Password
                                color: Theme.foreground
                                font.family: Theme.fontFamily
                                font.pixelSize: Theme.fontSizeNormal
                                
                                Text {
                                    anchors.fill: parent
                                    verticalAlignment: Text.AlignVCenter
                                    visible: apiInput.text === "" && !apiInput.activeFocus
                                    text: "Wprowadz klucz API..."
                                    color: Theme.mutedForeground
                                    font.family: Theme.fontFamily
                                    font.pixelSize: Theme.fontSizeNormal
                                }
                                
                                onTextChanged: root.apiKey = text
                            }
                        }
                        
                        Rectangle {
                            width: 44
                            height: 44
                            radius: Theme.radiusNormal
                            color: showApiBtn.containsMouse ? Theme.secondaryHover : Theme.secondary
                            border.width: 1
                            border.color: Theme.border
                            
                            Text {
                                anchors.centerIn: parent
                                text: showApiKey.checked ? "\uD83D\uDC41" : "\uD83D\uDC41\u200D\uD83D\uDDE8"
                                font.pixelSize: 18
                            }
                            
                            MouseArea {
                                id: showApiBtn
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: showApiKey.checked = !showApiKey.checked
                            }
                            
                            CheckBox {
                                id: showApiKey
                                visible: false
                            }
                        }
                    }
                }
            }
            
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: behaviorSection.implicitHeight + Theme.spacingMedium * 2
                radius: Theme.radiusNormal
                color: Theme.card
                border.width: 1
                border.color: Theme.border
                
                ColumnLayout {
                    id: behaviorSection
                    anchors.fill: parent
                    anchors.margins: Theme.spacingMedium
                    spacing: Theme.spacingNormal
                    
                    RowLayout {
                        spacing: Theme.spacingSmall
                        
                        Text {
                            text: "\u2699"
                            font.pixelSize: 18
                            color: Theme.primary
                        }
                        
                        Text {
                            text: "Zachowanie aplikacji"
                            color: Theme.foreground
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeMedium
                            font.weight: Font.Medium
                        }
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingNormal
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            Text { text: "Uruchom z systemem"; color: Theme.foreground; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSizeNormal }
                            Text { text: "Automatycznie uruchamia aplikacje przy starcie"; color: Theme.mutedForeground; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSizeSmall }
                        }
                        Rectangle {
                            width: 44; height: 24; radius: 12
                            color: root.startWithSystem ? Theme.primary : Theme.secondary
                            Rectangle {
                                x: root.startWithSystem ? parent.width - width - 2 : 2; y: 2; width: 20; height: 20; radius: 10; color: "#ffffff"
                                Behavior on x { NumberAnimation { duration: 150 } }
                            }
                            MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.startWithSystem = !root.startWithSystem }
                        }
                    }
                    
                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingNormal
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            Text { text: "Minimalizuj do zasobnika"; color: Theme.foreground; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSizeNormal }
                            Text { text: "Aplikacja pozostaje w zasobniku po zamknieciu"; color: Theme.mutedForeground; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSizeSmall }
                        }
                        Rectangle {
                            width: 44; height: 24; radius: 12
                            color: root.minimizeToTray ? Theme.primary : Theme.secondary
                            Rectangle {
                                x: root.minimizeToTray ? parent.width - width - 2 : 2; y: 2; width: 20; height: 20; radius: 10; color: "#ffffff"
                                Behavior on x { NumberAnimation { duration: 150 } }
                            }
                            MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.minimizeToTray = !root.minimizeToTray }
                        }
                    }
                }
            }
            
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingNormal
                
                Rectangle {
                    Layout.preferredWidth: 160; height: 40; radius: Theme.radiusNormal
                    color: resetBtn.containsMouse ? Theme.secondaryHover : Theme.secondary
                    border.width: 1; border.color: Theme.border
                    Text { anchors.centerIn: parent; text: "Przywroc domyslne"; color: Theme.foreground; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSizeNormal }
                    MouseArea { id: resetBtn; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: root.settingsReset() }
                }
                
                Item { Layout.fillWidth: true }
                
                Rectangle {
                    Layout.preferredWidth: 120; height: 40; radius: Theme.radiusNormal
                    color: saveBtn.containsMouse ? Theme.primaryHover : Theme.primary
                    Text { anchors.centerIn: parent; text: "Zapisz"; color: "#ffffff"; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSizeNormal; font.weight: Font.Medium }
                    MouseArea { id: saveBtn; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: root.settingsSaved() }
                }
            }
            
            Item { Layout.fillHeight: true }
        }
    }
}
