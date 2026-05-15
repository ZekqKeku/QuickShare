import QtQuick
import QtQuick.Controls
import QuickShare 1.0

Item {
    id: root
    
    property real value: 0.0
    property string speedText: ""
    property bool showPercentage: true
    property bool showSpeed: true
    property bool indeterminate: false
    property int barHeight: 8
    
    height: barHeight + (showPercentage || showSpeed ? 24 : 0)
    
    Row {
        id: statsRow
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: progressBackground.top
        anchors.bottomMargin: Theme.spacingTiny
        visible: showPercentage || showSpeed
        
        Text {
            visible: root.showPercentage
            text: Math.round(root.value * 100) + "%"
            color: Theme.foreground
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSizeSmall
            font.weight: Font.Medium
        }
        
        Item { width: Theme.spacingSmall; height: 1 }
        
        Text {
            visible: root.showSpeed && root.speedText !== ""
            text: root.speedText
            color: Theme.mutedForeground
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSizeSmall
        }
    }
    
    Rectangle {
        id: progressBackground
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: root.barHeight
        radius: root.barHeight / 2
        color: Theme.secondary
        border.width: 1
        border.color: Theme.border
        clip: true
        
        Rectangle {
            id: progressFill
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: root.indeterminate ? parent.width * 0.3 : parent.width * root.value
            radius: parent.radius
            
            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: Theme.primary }
                GradientStop { position: 1.0; color: Qt.lighter(Theme.primary, 1.2) }
            }
            
            Behavior on width {
                enabled: !root.indeterminate
                NumberAnimation {
                    duration: Theme.animationNormal
                    easing.type: Easing.OutCubic
                }
            }
            
            Rectangle {
                id: shineEffect
                width: 60
                height: parent.height
                radius: parent.radius
                opacity: 0.3
                
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0.0; color: "transparent" }
                    GradientStop { position: 0.5; color: "#ffffff" }
                    GradientStop { position: 1.0; color: "transparent" }
                }
                
                SequentialAnimation on x {
                    running: root.value > 0 && root.value < 1
                    loops: Animation.Infinite
                    NumberAnimation {
                        from: -60
                        to: progressFill.width
                        duration: 1500
                        easing.type: Easing.InOutQuad
                    }
                    PauseAnimation { duration: 500 }
                }
            }
            
            SequentialAnimation on x {
                running: root.indeterminate
                loops: Animation.Infinite
                NumberAnimation {
                    from: -progressFill.width
                    to: progressBackground.width
                    duration: 1200
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }
}
