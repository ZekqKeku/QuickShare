import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import QuickShare 1.0

Item {
    id: root
    
    property string apiKey: settingsManager.apiKey
    property string language: settingsManager.language
    property int themeMode: settingsManager.themeMode
    property string accentColor: settingsManager.accentColor
    property bool startWithSystem: settingsManager.startWithSystem
    property bool minimizeToTray: settingsManager.minimizeToTray

    onApiKeyChanged: { settingsManager.apiKey = apiKey; settingsManager.save() }
    onLanguageChanged: { settingsManager.language = language; settingsManager.save() }
    onThemeModeChanged: { settingsManager.themeMode = themeMode; settingsManager.save() }
    onAccentColorChanged: { settingsManager.accentColor = accentColor; settingsManager.save() }
    onStartWithSystemChanged: { settingsManager.startWithSystem = startWithSystem; settingsManager.save() }
    onMinimizeToTrayChanged: { settingsManager.minimizeToTray = minimizeToTray; settingsManager.save() }

    Binding { target: Theme; property: "accentColor"; value: root.accentColor }
    Binding { 
        target: Theme; 
        property: "isDarkMode"; 
        value: root.themeMode === 0 ? settingsManager.isSystemDarkMode : (root.themeMode === 2) 
    }

    signal settingsSaved()
    signal settingsReset()
    signal removeAllDataRequested()
    
    Rectangle {
        id: confirmDialog
        visible: false
        anchors.fill: parent
        color: "#AA000000"
        z: 100
        
        property string title: ""
        property string description: ""
        property string confirmText: ""
        property var onConfirm: null
        property bool isDestructive: true
        
        MouseArea { anchors.fill: parent }
        
        Rectangle {
            anchors.centerIn: parent
            width: Math.min(parent.width - 40, 360)
            height: dialogContent.implicitHeight + Theme.spacingLarge * 2
            radius: Theme.radiusLarge
            color: Theme.card
            border.width: 1
            border.color: Theme.border
            
            ColumnLayout {
                id: dialogContent
                anchors.fill: parent
                anchors.margins: Theme.spacingLarge
                spacing: Theme.spacingMedium
                
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: confirmDialog.title
                    color: Theme.foreground
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeLarge
                    font.weight: Font.Bold
                }
                
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    text: confirmDialog.description
                    color: Theme.mutedForeground
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeNormal
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingNormal
                    Button {
                        Layout.fillWidth: true
                        text: (settingsManager.translationContext, settingsManager.qsTr("cancel"))
                        onClicked: confirmDialog.visible = false
                        
                        contentItem: Text {
                            text: parent.text
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeNormal
                            color: Theme.foreground
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        background: Rectangle {
                            implicitHeight: 38
                            color: parent.hovered ? Theme.cardHover : Theme.card
                            border.color: Theme.border
                            border.width: 1
                            radius: Theme.radiusNormal
                        }
                    }
                    Button {
                        Layout.fillWidth: true
                        text: confirmDialog.confirmText
                        onClicked: {
                            confirmDialog.visible = false
                            if (confirmDialog.onConfirm) confirmDialog.onConfirm()
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeNormal
                            color: "#ffffff"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        background: Rectangle {
                            implicitHeight: 38
                            color: parent.hovered ? (confirmDialog.isDestructive ? Qt.darker(Theme.error, 1.1) : Qt.darker(Theme.primary, 1.1)) : (confirmDialog.isDestructive ? Theme.error : Theme.primary)
                            radius: Theme.radiusNormal
                        }
                    }
                }
            }
        }
    }
    
    ScrollView {
        id: settingsScroll
        anchors.fill: parent
        clip: true
        ScrollBar.vertical.policy: ScrollBar.AsNeeded
        
        Item {
            id: centeredCanvas
            width: settingsScroll.availableWidth
            implicitHeight: mainColumn.implicitHeight + Theme.spacingLarge * 2
            
            ColumnLayout {
                id: mainColumn
                width: Math.min(centeredCanvas.width - 40, 380)
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: Theme.spacingMedium
                spacing: Theme.spacingLarge

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingSmall
                    
                    Text { 
                        Layout.alignment: Qt.AlignHCenter
                        text: (settingsManager.translationContext, settingsManager.qsTr("api_section_title"))
                        color: Theme.foreground; font.pixelSize: Theme.fontSizeMedium; font.weight: Font.Medium 
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingSmall

                        TextField {
                            id: apiField
                            Layout.fillWidth: true
                            text: root.apiKey
                            placeholderText: (settingsManager.translationContext, settingsManager.qsTr("api_key_placeholder"))
                            echoMode: showApiBtn.checked ? TextInput.Normal : TextInput.Password
                            onTextChanged: root.apiKey = text
                            
                            color: Theme.foreground
                            placeholderTextColor: Theme.mutedForeground
                            selectionColor: Theme.primary
                            selectedTextColor: "#ffffff"
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeNormal
                            
                            background: Rectangle {
                                implicitHeight: 38
                                color: Theme.input
                                border.color: apiField.activeFocus ? Theme.primary : Theme.inputBorder
                                border.width: apiField.activeFocus ? 2 : 1
                                radius: Theme.radiusNormal
                            }

                            Button {
                                id: showApiBtn
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.rightMargin: 8
                                width: 32; height: 32
                                checkable: true
                                flat: true
                                
                                contentItem: Item {
                                    id: iconContainer
                                    implicitWidth: 24
                                    implicitHeight: 20

                                    Shape {
                                        anchors.fill: parent
                                        vendorExtensionsEnabled: true
                                        layer.enabled: true
                                        layer.samples: 4
                                        
                                        ShapePath {
                                            strokeColor: showApiBtn.checked ? Theme.primary : Theme.mutedForeground
                                            strokeWidth: 1.5
                                            fillColor: "transparent"
                                            capStyle: ShapePath.RoundCap
                                            joinStyle: ShapePath.RoundJoin

                                            startX: 0; startY: 10
                                            PathQuad { x: 16; y: 10; controlX: 8; controlY: 2 }
                                            PathQuad { x: 0; y: 10; controlX: 8; controlY: 18 }
                                        }

                                        ShapePath {
                                            fillColor: showApiBtn.checked ? Theme.primary : "transparent"
                                            strokeColor: showApiBtn.checked ? Theme.primary : Theme.mutedForeground
                                            strokeWidth: 1.5
                                            PathAngleArc { centerX: 8; centerY: 10; radiusX: 2.5; radiusY: 2.5; startAngle: 0; sweepAngle: 360 }
                                        }

                                        ShapePath {
                                            strokeColor: !showApiBtn.checked ? Theme.mutedForeground : "transparent"
                                            strokeWidth: 1.5
                                            capStyle: ShapePath.RoundCap
                                            startX: 3; startY: 5
                                            PathLine { x: 13; y: 15 }
                                        }
                                    }
                                }
                                
                                background: Rectangle {
                                    color: parent.hovered ? (Theme.isDarkMode ? "#1FFFFFFF" : "#1F000000") : "transparent"
                                    radius: 16
                                }
                            }
                        }
                    }
                    Text { 
                        Layout.alignment: Qt.AlignHCenter
                        text: (settingsManager.translationContext, settingsManager.qsTr("api_key_hint"))
                        color: Theme.mutedForeground; font.pixelSize: Theme.fontSizeSmall 
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingMedium
                    
                    Text { 
                        Layout.alignment: Qt.AlignHCenter
                        text: (settingsManager.translationContext, settingsManager.qsTr("appearance_section_title")); color: Theme.foreground; font.pixelSize: Theme.fontSizeMedium; font.weight: Font.Medium 
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingSmall
                        
                        Repeater {
                            model: [
                                { label: (settingsManager.translationContext, settingsManager.qsTr("theme_dark")), value: 2 },
                                { label: (settingsManager.translationContext, settingsManager.qsTr("theme_light")), value: 1 },
                                { label: (settingsManager.translationContext, settingsManager.qsTr("theme_system")), value: 0 }
                            ]
                            Button {
                                Layout.fillWidth: true
                                text: modelData.label
                                checkable: true
                                checked: root.themeMode === modelData.value
                                onClicked: root.themeMode = modelData.value
                                
                                contentItem: Text {
                                    text: parent.text
                                    font.family: Theme.fontFamily
                                    font.pixelSize: Theme.fontSizeNormal
                                    color: parent.checked ? "#ffffff" : Theme.foreground
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                
                                background: Rectangle {
                                    implicitHeight: 36
                                    color: parent.checked ? Theme.primary : (parent.hovered ? Theme.cardHover : Theme.card)
                                    border.color: parent.checked ? Theme.primary : Theme.border
                                    border.width: 1
                                    radius: Theme.radiusNormal
                                    
                                    Behavior on color { ColorAnimation { duration: 150 } }
                                }
                            }
                        }
                    }
                    
                    Text { 
                        Layout.alignment: Qt.AlignHCenter
                        text: (settingsManager.translationContext, settingsManager.qsTr("accent_color_label")); color: Theme.foreground; font.pixelSize: Theme.fontSizeSmall; font.weight: Font.Medium 
                    }
                    
                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        spacing: Theme.spacingSmall
                        
                        Row {
                            spacing: 12
                            Repeater {
                                model: Theme.accentColors
                                Item {
                                    width: 36; height: 36
                                    
                                    readonly property bool isSelected: {
                                        var current = String(root.accentColor).toUpperCase();
                                        return current === String(modelData.dark).toUpperCase() || 
                                               current === String(modelData.light).toUpperCase();
                                    }

                                    Canvas {
                                        id: colorCircle
                                        anchors.fill: parent
                                        antialiasing: true
                                        onPaint: {
                                            var ctx = getContext("2d");
                                            ctx.reset();
                                            
                                            var centerX = width / 2;
                                            var centerY = height / 2;
                                            var radius = width / 2 - 4;

                                            ctx.fillStyle = modelData.dark;
                                            ctx.beginPath();
                                            ctx.arc(centerX, centerY, radius, Math.PI / 2, 3 * Math.PI / 2);
                                            ctx.fill();

                                            ctx.fillStyle = modelData.light;
                                            ctx.beginPath();
                                            ctx.arc(centerX, centerY, radius, 3 * Math.PI / 2, Math.PI / 2);
                                            ctx.fill();

                                            if (isSelected) {
                                                ctx.strokeStyle = Theme.foreground;
                                                ctx.lineWidth = 2.5;
                                                ctx.beginPath();
                                                ctx.arc(centerX, centerY, radius + 2, 0, 2 * Math.PI);
                                                ctx.stroke();
                                            }
                                        }

                                        Connections {
                                            target: root
                                            function onAccentColorChanged() { colorCircle.requestPaint() }
                                        }
                                        Connections {
                                            target: Theme
                                            function onForegroundChanged() { colorCircle.requestPaint() }
                                            function onIsDarkModeChanged() { colorCircle.requestPaint() }
                                        }
                                    }

                                    MouseArea { 
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            var isDark = (root.themeMode === 0 ? settingsManager.isSystemDarkMode : root.themeMode === 2);
                                            root.accentColor = isDark ? modelData.dark : modelData.light;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    TextField {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 120
                        text: root.accentColor
                        placeholderText: "#HEX"
                        onTextChanged: if (text.length >= 4) root.accentColor = text
                        horizontalAlignment: Text.AlignHCenter
                        
                        color: Theme.foreground
                        placeholderTextColor: Theme.mutedForeground
                        selectionColor: Theme.primary
                        selectedTextColor: "#ffffff"
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontSizeNormal
                        
                        background: Rectangle {
                            implicitHeight: 38
                            color: Theme.input
                            border.color: parent.activeFocus ? Theme.primary : Theme.inputBorder
                            border.width: parent.activeFocus ? 2 : 1
                            radius: Theme.radiusNormal
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingSmall
                    
                    Text { 
                        Layout.alignment: Qt.AlignHCenter
                        text: (settingsManager.translationContext, settingsManager.qsTr("behavior_section_title")); 
                        color: Theme.foreground; font.pixelSize: Theme.fontSizeMedium; font.weight: Font.Medium 
                    }
                    
                        CheckBox {
                            Layout.fillWidth: true
                            text: (settingsManager.translationContext, settingsManager.qsTr("start_with_system"))
                            checked: root.startWithSystem
                            onCheckedChanged: root.startWithSystem = checked
                            
                            indicator: Rectangle {
                                implicitWidth: 20
                                implicitHeight: 20
                                y: Math.round((parent.height - height) / 2)
                                radius: 4
                                color: parent.checked ? Theme.primary : "transparent"
                                border.color: parent.checked ? Theme.primary : Theme.border
                                border.width: 1

                                Text {
                                    anchors.centerIn: parent
                                    text: "\u2713"
                                    color: "white"
                                    visible: parent.parent.checked
                                    font.bold: true
                                }
                            }
                            
                            contentItem: Text {
                                text: parent.text
                                font.family: Theme.fontFamily
                                font.pixelSize: Theme.fontSizeNormal
                                color: Theme.foreground
                                leftPadding: parent.indicator.width + parent.spacing
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                        
                        CheckBox {
                            Layout.fillWidth: true
                            text: Qt.platform.os === "macos" || Qt.platform.os === "osx" 
                                ? (settingsManager.translationContext, settingsManager.qsTr("minimize_to_tray_macos")) 
                                : (settingsManager.translationContext, settingsManager.qsTr("minimize_to_tray_other"))
                            checked: root.minimizeToTray
                            onCheckedChanged: root.minimizeToTray = checked
                            
                            indicator: Rectangle {
                                implicitWidth: 20
                                implicitHeight: 20
                                y: Math.round((parent.height - height) / 2)
                                radius: 4
                                color: parent.checked ? Theme.primary : "transparent"
                                border.color: parent.checked ? Theme.primary : Theme.border
                                border.width: 1

                                Text {
                                    anchors.centerIn: parent
                                    text: "\u2713"
                                    color: "white"
                                    visible: parent.parent.checked
                                    font.bold: true
                                }
                            }
                            
                            contentItem: Text {
                                text: parent.text
                                font.family: Theme.fontFamily
                                font.pixelSize: Theme.fontSizeNormal
                                color: Theme.foreground
                                leftPadding: parent.indicator.width + parent.spacing
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingSmall
                    
                    Text { 
                        Layout.alignment: Qt.AlignHCenter
                        text: (settingsManager.translationContext, settingsManager.qsTr("language_section_title")); color: Theme.foreground; font.pixelSize: Theme.fontSizeMedium; font.weight: Font.Medium 
                    }
                    
                    ComboBox {
                        id: langCombo
                        Layout.fillWidth: true
                        model: settingsManager.availableLanguages
                        textRole: "name"
                        currentIndex: {
                            for(var i=0; i<model.length; i++) {
                                if(model[i].code === root.language) return i;
                            }
                            return 0;
                        }
                        onActivated: {
                            root.language = model[index].code
                        }
                        
                        contentItem: Text {
                            leftPadding: 12
                            rightPadding: langCombo.indicator.width + langCombo.spacing
                            text: langCombo.displayText
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeNormal
                            color: Theme.foreground
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                        
                        background: Rectangle {
                            implicitHeight: 38
                            color: Theme.card
                            border.color: langCombo.activeFocus ? Theme.primary : Theme.border
                            border.width: langCombo.activeFocus ? 2 : 1
                            radius: Theme.radiusNormal
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingSmall
                    
                    Text { 
                        Layout.alignment: Qt.AlignHCenter
                        text: (settingsManager.translationContext, settingsManager.qsTr("data_section_title")); color: Theme.foreground; font.pixelSize: Theme.fontSizeMedium; font.weight: Font.Medium 
                    }
                    
                    Button {
                        text: (settingsManager.translationContext, settingsManager.qsTr("reset_defaults"))
                        Layout.fillWidth: true
                        onClicked: {
                            confirmDialog.title = (settingsManager.translationContext, settingsManager.qsTr("confirm_reset_title"))
                            confirmDialog.description = (settingsManager.translationContext, settingsManager.qsTr("confirm_reset_text"))
                            confirmDialog.confirmText = (settingsManager.translationContext, settingsManager.qsTr("reset_defaults"))
                            confirmDialog.isDestructive = false
                            confirmDialog.onConfirm = function() {
                                settingsManager.resetToDefaults()
                                
                                root.apiKey = settingsManager.apiKey
                                root.language = settingsManager.language
                                root.themeMode = settingsManager.themeMode
                                root.accentColor = settingsManager.accentColor
                                root.startWithSystem = settingsManager.startWithSystem
                                root.minimizeToTray = settingsManager.minimizeToTray
                            }
                            confirmDialog.visible = true
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeNormal
                            color: Theme.foreground
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        background: Rectangle {
                            implicitHeight: 38
                            color: parent.hovered ? Theme.cardHover : Theme.card
                            border.color: Theme.border
                            border.width: 1
                            radius: Theme.radiusNormal
                        }
                    }
                    Button {
                       text: (settingsManager.translationContext, settingsManager.qsTr("clear_all_data"))
                       Layout.fillWidth: true
                       onClicked: {
                           confirmDialog.title = (settingsManager.translationContext, settingsManager.qsTr("confirm_delete_title"))
                           confirmDialog.description = (settingsManager.translationContext, settingsManager.qsTr("confirm_delete_text"))
                           confirmDialog.confirmText = (settingsManager.translationContext, settingsManager.qsTr("delete_data"))
                           confirmDialog.isDestructive = true
                           confirmDialog.onConfirm = function() { root.removeAllDataRequested() }
                           confirmDialog.visible = true
                       }
                        contentItem: Text {
                            text: parent.text
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeNormal
                            color: Theme.error
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        background: Rectangle {
                            implicitHeight: 38
                            color: parent.hovered ? Qt.alpha(Theme.error, 0.1) : "transparent"
                            border.color: Theme.error
                            border.width: 1
                            radius: Theme.radiusNormal
                        }
                    }
                }
            }
        }
    }
}
