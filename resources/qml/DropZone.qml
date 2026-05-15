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
        title: (settingsManager.translationContext, settingsManager.qsTr("select_files_dialog"))
        onAccepted: {
            root.filesDropped(selectedFiles)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingMedium
        spacing: Theme.spacingMedium
        
        Rectangle {
            id: dropArea
            Layout.fillWidth: true
            Layout.preferredHeight: root.isUploading || root.uploadedFileUrl !== "" ? 120 : parent.height * 0.6
            radius: Theme.radiusLarge
            color: dropMouseArea.containsDrag ? Theme.secondaryHover : Theme.secondary
            border.width: 2
            border.color: dropMouseArea.containsDrag ? Theme.primary : Theme.border
            
            Behavior on Layout.preferredHeight {
                NumberAnimation { duration: Theme.animationNormal; easing.type: Easing.OutCubic }
            }
            
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
                onDropped: (drop) => {
                    if (drop.hasUrls) {
                        root.filesDropped(drop.urls)
                    }
                }
            }
            
            ColumnLayout {
                anchors.centerIn: parent
                spacing: Theme.spacingSmall
                visible: !root.isUploading && root.uploadedFileUrl === ""
                
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 48; height: 48; radius: 24
                    color: Theme.card
                    border.width: 1; border.color: Theme.border
                    Text { anchors.centerIn: parent; text: "\u2191"; font.pixelSize: 24; color: Theme.primary }
                }
                
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: (settingsManager.translationContext, settingsManager.qsTr("drop_files_here"))
                    color: Theme.foreground
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeMedium
                    font.weight: Font.DemiBold
                }
                
                Button {
                    Layout.alignment: Qt.AlignHCenter
                    text: (settingsManager.translationContext, settingsManager.qsTr("select_files"))
                    onClicked: fileDialog.open()
                    
                    contentItem: Text {
                        text: parent.text
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontSizeNormal
                        color: Theme.foreground
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        leftPadding: Theme.spacingMedium
                        rightPadding: Theme.spacingMedium
                    }
                    
                    background: Rectangle {
                        implicitHeight: 36
                        color: parent.hovered ? Theme.cardHover : Theme.card
                        border.color: Theme.border
                        border.width: 1
                        radius: Theme.radiusNormal
                    }
                }
            }

            RowLayout {
                anchors.centerIn: parent
                width: parent.width - 40
                visible: root.isUploading
                spacing: Theme.spacingMedium
                
                Rectangle {
                    width: 40; height: 40; radius: Theme.radiusNormal; color: Theme.card
                    Text { anchors.centerIn: parent; text: "\uD83D\uDCC4"; font.pixelSize: 20; color: Theme.primary }
                }
                
                ColumnLayout {
                    Layout.fillWidth: true; spacing: 2
                    Text { text: root.uploadedFileName; color: Theme.foreground; font.pixelSize: Theme.fontSizeSmall; font.weight: Font.Medium; elide: Text.ElideMiddle; Layout.fillWidth: true }
                    CustomProgressBar { Layout.fillWidth: true; value: root.uploadProgress; barHeight: 4; showPercentage: false; showSpeed: false }
                    Text { text: (settingsManager.translationContext, settingsManager.qsTr("uploading")) + root.uploadSpeed; color: Theme.mutedForeground; font.pixelSize: 10 }
                }
            }

            ColumnLayout {
                anchors.centerIn: parent
                width: parent.width - 40
                visible: !root.isUploading && root.uploadedFileUrl !== ""
                spacing: 8
                
                Text { Layout.alignment: Qt.AlignHCenter; text: (settingsManager.translationContext, settingsManager.qsTr("upload_success")); color: Theme.success; font.pixelSize: Theme.fontSizeMedium; font.weight: Font.Bold }
                
                RowLayout {
                    Layout.fillWidth: true
                    TextField {
                        id: urlField
                        Layout.fillWidth: true
                        text: root.uploadedFileUrl
                        readOnly: true
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.foreground
                        selectionColor: Theme.primary
                        selectedTextColor: "#ffffff"
                        
                        background: Rectangle {
                            implicitHeight: 36
                            color: Theme.input
                            border.color: Theme.border
                            border.width: 1
                            radius: Theme.radiusNormal
                        }
                    }
                    Button {
                        text: (settingsManager.translationContext, settingsManager.qsTr("copy"))
                        onClicked: {
                            clipboard.setText(root.uploadedFileUrl)
                            urlField.selectAll()
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeNormal
                            color: Theme.foreground
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            leftPadding: Theme.spacingMedium
                            rightPadding: Theme.spacingMedium
                        }
                        
                        background: Rectangle {
                            implicitHeight: 36
                            color: parent.hovered ? Theme.cardHover : Theme.card
                            border.color: Theme.border
                            border.width: 1
                            radius: Theme.radiusNormal
                        }
                    }
                }
                
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: (settingsManager.translationContext, settingsManager.qsTr("send_another"))
                    color: Theme.primary
                    font.pixelSize: Theme.fontSizeSmall
                    font.underline: true
                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: (mouse) => uploadManager.clearLastUpload() }
                }
            }
        }
        
        Item { Layout.fillHeight: true }
    }
}
