import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QuickShare

Window {
    id: mainWindow
    
    visible: true
    width: 520
    height: 580
    minimumWidth: 420
    minimumHeight: 480
    
    flags: Qt.Window | Qt.FramelessWindowHint
    color: "transparent"
    
    title: "QuickShare"
    
    MainWindow {
        anchors.fill: parent
        anchors.margins: 8
    }
    
    Component {
        id: uploadPopupComponent
        
        UploadPopup {
            onCopyClicked: {
                clipboard.setText(fileUrl)
            }
            
            onCancelClicked: {
                close()
            }
            
            onCloseRequested: {
                close()
                destroy()
            }
        }
    }
    
    function showUploadPopup(fileName, fileSize) {
        var popup = uploadPopupComponent.createObject(null, {
            fileName: fileName,
            fileSize: fileSize,
            x: Screen.width - 400,
            y: Screen.height - 250,
            uploadProgress: Qt.binding(function() { return uploadManager.progress }),
            uploadSpeed: Qt.binding(function() { return uploadManager.speed }),
            fileUrl: Qt.binding(function() { return uploadManager.lastUploadUrl })
        })
        
        var onCompleted = function(finishedFileName, url) {
            if (finishedFileName === fileName) {
                popup.isCompleted = true
                uploadManager.uploadCompleted.disconnect(onCompleted)
            }
        }
        uploadManager.uploadCompleted.connect(onCompleted)
        
        popup.show()
        return popup
    }
}
