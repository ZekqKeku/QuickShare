// main.qml - Punkt wejsciowy aplikacji
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

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
    
    // Glowne okno aplikacji
    MainWindow {
        anchors.fill: parent
        anchors.margins: 8
    }
    
    // Popup uploadu (tworzony dynamicznie)
    Component {
        id: uploadPopupComponent
        
        UploadPopup {
            onCopyClicked: {
                // Kopiuj link do schowka
                clipboard.setText(fileUrl)
            }
            
            onCancelClicked: {
                // Anuluj upload
                close()
            }
            
            onCloseRequested: {
                close()
                destroy()
            }
        }
    }
    
    // Funkcja do pokazania popupu uploadu
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
        
        // Polacz sygnal zakonczenia uploadu
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
