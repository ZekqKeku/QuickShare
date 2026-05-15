import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickShare 1.0

WindowFrame {
    id: root
    
    title: "QuickShare"
    subtitle: tabBar.currentIndex === 0 ? "Przeslij" : "Ustawienia"
    
    onMinimizeClicked: Window.window.showMinimized()
    onMaximizeClicked: {
        if (Window.window.visibility === Window.Maximized) {
            Window.window.showNormal()
        } else {
            Window.window.showMaximized()
        }
    }
    onCloseClicked: Qt.quit()
    
    content: Item {
        anchors.fill: parent
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            
            Rectangle {
                Layout.fillWidth: true
                height: 48
                color: Theme.card
                opacity: 0.8
                
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: Theme.spacingMedium
                    anchors.rightMargin: Theme.spacingMedium
                    spacing: 0
                    
                    Item {
                        Layout.preferredWidth: 100
                        Layout.fillHeight: true
                        
                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0
                            
                            Item { Layout.fillHeight: true }
                            
                            RowLayout {
                                Layout.alignment: Qt.AlignHCenter
                                spacing: Theme.spacingSmall
                                
                                Text {
                                    text: "\u2191"
                                    color: tabBar.currentIndex === 0 ? Theme.primary : Theme.mutedForeground
                                    font.pixelSize: 16
                                    font.weight: tabBar.currentIndex === 0 ? Font.Bold : Font.Normal
                                }
                                
                                Text {
                                    text: "Przeslij"
                                    color: tabBar.currentIndex === 0 ? Theme.primary : Theme.mutedForeground
                                    font.family: Theme.fontFamily
                                    font.pixelSize: Theme.fontSizeNormal
                                    font.weight: tabBar.currentIndex === 0 ? Font.Medium : Font.Normal
                                }
                            }
                            
                            Item { Layout.fillHeight: true }
                            
                            Rectangle {
                                Layout.fillWidth: true
                                height: 2
                                color: tabBar.currentIndex === 0 ? Theme.primary : "transparent"
                                visible: tabBar.currentIndex === 0
                            }
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            hoverEnabled: true
                            onClicked: tabBar.currentIndex = 0
                        }
                    }
                    
                    Item {
                        Layout.preferredWidth: 110
                        Layout.fillHeight: true
                        
                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0
                            
                            Item { Layout.fillHeight: true }
                            
                            RowLayout {
                                Layout.alignment: Qt.AlignHCenter
                                spacing: Theme.spacingSmall
                                
                                Text {
                                    text: "\u2699"
                                    color: tabBar.currentIndex === 1 ? Theme.primary : Theme.mutedForeground
                                    font.pixelSize: 16
                                    font.weight: tabBar.currentIndex === 1 ? Font.Bold : Font.Normal
                                }
                                
                                Text {
                                    text: "Ustawienia"
                                    color: tabBar.currentIndex === 1 ? Theme.primary : Theme.mutedForeground
                                    font.family: Theme.fontFamily
                                    font.pixelSize: Theme.fontSizeNormal
                                    font.weight: tabBar.currentIndex === 1 ? Font.Medium : Font.Normal
                                }
                            }
                            
                            Item { Layout.fillHeight: true }
                            
                            Rectangle {
                                Layout.fillWidth: true
                                height: 2
                                color: tabBar.currentIndex === 1 ? Theme.primary : "transparent"
                                visible: tabBar.currentIndex === 1
                            }
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            hoverEnabled: true
                            onClicked: tabBar.currentIndex = 1
                        }
                    }
                    
                    Item { Layout.fillWidth: true }
                }
            }
            
            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: Theme.border
            }
            
            StackLayout {
                id: tabBar
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: 0
                
                DropZone {
                    id: dropZone
                    isUploading: uploadManager.isUploading
                    uploadProgress: uploadManager.progress
                    uploadSpeed: uploadManager.speed
                    uploadedFileUrl: uploadManager.lastUploadUrl
                    uploadedFileName: uploadManager.currentFileName
                    
                    onFilesDropped: {
                        uploadManager.uploadFiles(urls)
                    }
                }
                
                SettingsPanel {
                    id: settingsPanel
                    
                    apiKey: settingsManager.apiKey
                    language: settingsManager.language
                    startWithSystem: settingsManager.startWithSystem
                    minimizeToTray: settingsManager.minimizeToTray
                    
                    onSettingsSaved: {
                        settingsManager.apiKey = apiKey
                        settingsManager.language = language
                        settingsManager.startWithSystem = startWithSystem
                        settingsManager.minimizeToTray = minimizeToTray
                        settingsManager.save()
                    }
                    
                    onSettingsReset: {
                        settingsManager.resetToDefaults()
                    }
                    
                    onRemoveAllDataRequested: {
                        if (typeof settingsManager.removeAllData === "function") {
                            settingsManager.removeAllData()
                        }
                    }
                }
            }
        }
    }
}
