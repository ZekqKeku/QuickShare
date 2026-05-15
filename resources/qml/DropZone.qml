import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QuickShare 1.0

Item {
    id: root
    
    signal filesDropped(var urls)
    
    property bool isUploading: false
    property real uploadProgress: 0.0
    property string uploadSpeed: ""
    property string uploadedFileUrl: ""
    property string uploadedFileName: ""
    
    FileDialog {
        id: fileDialog
        title: "Wybierz pliki do przeslania"
        onAccepted: {
            root.filesDropped(selectedFiles)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLarge
        spacing: Theme.spacingLarge
        
        Rectangle {
            id: dropArea
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.radiusLarge
            color: dropMouseArea.containsDrag ? Theme.secondaryHover : Theme.secondary
            border.width: 2
            border.color: dropMouseArea.containsDrag ? Theme.primary : Theme.border
            
            Canvas {
                anchors.fill: parent
                anchors.margins: 2
                visible: !root.isUploading && root.uploadedFileUrl === ""
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.reset();
                    ctx.strokeStyle = Theme.border;
                    ctx.lineWidth = 2;
                    ctx.setLineDash([8, 8]);
                    ctx.beginPath();
                    ctx.roundedRect(2, 2, width - 4, height - 4, Theme.radiusLarge, Theme.radiusLarge);
                    ctx.stroke();
                }
            }
            
            DropArea {
                id: dropMouseArea
                anchors.fill: parent
                onDropped: {
                    if (drop.hasUrls) {
                        root.filesDropped(drop.urls)
                    }
                }
            }
            
            ColumnLayout {
                anchors.centerIn: parent
                spacing: Theme.spacingMedium
                visible: !root.isUploading && root.uploadedFileUrl === ""
                
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 64; height: 64; radius: 32
                    color: Theme.card
                    border.width: 1; border.color: Theme.border
                    Text { anchors.centerIn: parent; text: "\u2191"; font.pixelSize: 32; color: Theme.primary }
                }
                
                ColumnLayout {
                    spacing: 4
                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: "Przeciagnij i upusc pliki"
                        color: Theme.foreground
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontSizeLarge
                        font.weight: Font.SemiBold
                    }
                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: "Obslugujemy wszystkie formaty plikow"
                        color: Theme.mutedForeground
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontSizeSmall
                    }
                }
                
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 160; height: 40; radius: Theme.radiusNormal
                    color: selectBtnArea.containsMouse ? Theme.primaryHover : Theme.primary
                    Text { anchors.centerIn: parent; text: "Wybierz pliki"; color: "#ffffff"; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSizeNormal; font.weight: Font.Medium }
                    MouseArea { id: selectBtnArea; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: fileDialog.open() }
                }
            }
            
            ColumnLayout {
                anchors.centerIn: parent
                width: parent.width - 80
                spacing: Theme.spacingLarge
                visible: root.isUploading
                
                RowLayout {
                    spacing: Theme.spacingMedium
                    Rectangle {
                        width: 48; height: 48; radius: Theme.radiusNormal; color: Theme.card
                        Text { anchors.centerIn: parent; text: "\uD83D\uDCC4"; font.pixelSize: 24; color: Theme.primary }
                    }
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2
                        Text { text: root.uploadedFileName; color: Theme.foreground; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSizeMedium; font.weight: Font.Medium; elide: Text.ElideMiddle; Layout.fillWidth: true }
                        Text { text: "Przesylanie... " + root.uploadSpeed; color: Theme.mutedForeground; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSizeSmall }
                    }
                }
                
                CustomProgressBar { Layout.fillWidth: true; value: root.uploadProgress; barHeight: 8 }
            }
            
            ColumnLayout {
                anchors.centerIn: parent
                width: parent.width - 80
                spacing: Theme.spacingLarge
                visible: !root.isUploading && root.uploadedFileUrl !== ""
                
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 64; height: 64; radius: 32; color: Qt.rgba(Theme.success.r, Theme.success.g, Theme.success.b, 0.1)
                    Text { anchors.centerIn: parent; text: "\u2713"; font.pixelSize: 32; color: Theme.success }
                }
                
                ColumnLayout {
                    spacing: Theme.spacingSmall
                    Text { Layout.alignment: Qt.AlignHCenter; text: "Przeslano pomyslnie!"; color: Theme.foreground; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSizeLarge; font.weight: Font.Bold }
                    
                    Rectangle {
                        Layout.preferredWidth: parent.parent.width; height: 44; radius: Theme.radiusNormal; color: Theme.card; border.width: 1; border.color: Theme.border
                        RowLayout {
                            anchors.fill: parent; anchors.margins: 1; spacing: 0
                            TextInput { Layout.fillWidth: true; Layout.leftMargin: Theme.spacingMedium; text: root.uploadedFileUrl; color: Theme.foreground; font.family: Theme.fontFamily; readOnly: true; verticalAlignment: Text.AlignVCenter }
                            Rectangle {
                                width: 100; height: parent.height; radius: Theme.radiusNormal; color: copyBtnArea2.containsMouse ? Theme.primaryHover : Theme.primary
                                Text { anchors.centerIn: parent; text: "Kopiuj"; color: "#ffffff"; font.weight: Font.Medium }
                                MouseArea { id: copyBtnArea2; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: { clipboard.setText(root.uploadedFileUrl); } }
                            }
                        }
                    }
                }
                
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Wroc do startu"
                    color: Theme.mutedForeground
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeSmall
                    font.underline: true
                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: { root.uploadedFileUrl = ""; root.uploadedFileName = ""; } }
                }
            }
        }
    }
}
