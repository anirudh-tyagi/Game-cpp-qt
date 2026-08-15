import QtQuick

//The one calm page shown before a run and after one ends. It carries no game
//state, the caller decides what the wording is and what the buttons do.
Item {
    id: overlay

    property string title: ""
    property string subtitle: ""
    property string hint: ""
    property string actionText: qsTr("Start")

    signal actionClicked()
    signal quitClicked()

    //swallow clicks and keeps the play field from reacting behind the menu
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
    }

    //soft paper-to-sage wash instead of a flat dim
    Rectangle {
        anchors.fill: parent

        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.menuVeilTop }
            GradientStop { position: 1.0; color: Theme.menuVeilBottom }
        }
    }

    Rectangle {
        id: panel

        //how far the panel still has to travel to reach its resting place,
        //animated instead of y itself so the centring binding stays intact
        property real settle: 0

        anchors.horizontalCenter: parent.horizontalCenter
        y: ((overlay.height - height) / 2) + settle
        width: Theme.menuPanelWidth
        height: content.implicitHeight + (2 * Theme.menuPanelPadding)
        radius: Theme.menuRadius
        color: Theme.menuPanel
        border.color: Theme.menuPanelBorder
        border.width: 1

        Column {
            id: content

            anchors.centerIn: parent
            width: parent.width - (2 * Theme.menuPanelPadding)
            spacing: Theme.menuSpacing

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: overlay.title
                color: Theme.menuTitleColor
                font.family: Theme.displayFont.name
                font.pixelSize: Theme.menuTitleFontSize
                font.letterSpacing: 2
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                text: overlay.subtitle
                color: Theme.menuSubtitleColor
                font.family: Theme.bodyFont.name
                font.pixelSize: Theme.menuSubtitleFontSize
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            //a little air before the buttons so the page does not feel crowded
            Item {
                width: 1
                height: Theme.menuSpacing
            }

            MenuButton {
                anchors.horizontalCenter: parent.horizontalCenter
                text: overlay.actionText
                onClicked: overlay.actionClicked()
            }

            MenuButton {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Quit")
                quiet: true
                onClicked: overlay.quitClicked()
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                text: overlay.hint
                color: Theme.menuHintColor
                font.family: Theme.bodyFont.name
                font.pixelSize: Theme.menuHintFontSize
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                topPadding: Theme.menuSpacing
            }
        }
    }

    //the page settles in rather than appearing, which is what makes the whole
    //thing read as unhurried
    onVisibleChanged: if (visible) enter.restart()

    Component.onCompleted: if (visible) enter.restart()

    ParallelAnimation {
        id: enter

        NumberAnimation {
            target: panel
            property: "settle"
            from: 18
            to: 0
            duration: Theme.menuFadeMs
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            target: overlay
            property: "opacity"
            from: 0
            to: 1
            duration: Theme.menuFadeMs
            easing.type: Easing.OutCubic
        }
    }
}
