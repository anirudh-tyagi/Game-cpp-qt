import QtQuick

//A soft pill button for the menu. Pure view, it only reports clicks.
Rectangle {
    id: button

    property string text: ""
    //the quiet variant recedes, used for the action we do not want to nudge
    //anyone towards
    property bool quiet: false

    signal clicked()

    implicitWidth: Theme.buttonWidth
    implicitHeight: Theme.buttonHeight
    radius: Theme.buttonRadius

    color: {
        if (mouse.pressed)
            return button.quiet ? Theme.buttonQuietPressedColor : Theme.buttonPressedColor
        if (mouse.containsMouse)
            return button.quiet ? Theme.buttonQuietHoverColor : Theme.buttonHoverColor
        return button.quiet ? Theme.buttonQuietColor : Theme.buttonColor
    }

    //no hard state flips, the colour eases between the three states
    Behavior on color {
        ColorAnimation {
            duration: Theme.buttonFadeMs
            easing.type: Easing.OutQuad
        }
    }

    //a barely-there press, enough to feel responsive without being springy
    scale: mouse.pressed ? 0.98 : 1.0
    Behavior on scale {
        NumberAnimation {
            duration: Theme.buttonFadeMs
            easing.type: Easing.OutQuad
        }
    }

    Text {
        anchors.centerIn: parent
        text: button.text
        color: button.quiet ? Theme.buttonQuietTextColor : Theme.buttonTextColor
        font.family: Theme.bodyFont.name
        font.pixelSize: Theme.buttonFontSize
        font.letterSpacing: 1
    }

    MouseArea {
        id: mouse

        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: button.clicked()
    }
}
