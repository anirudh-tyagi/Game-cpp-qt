import QtQuick

//Transient level-up announcement. It holds no state of its own, the caller
//tells it when to speak by calling announce().
Item {
    id: banner

    property int level: 1
    property string note: ""

    //sizes to its text, so the caller can place it by width alone
    implicitWidth: column.implicitWidth
    implicitHeight: column.implicitHeight

    //nothing on screen until announce() runs
    opacity: 0
    visible: opacity > 0

    function announce() {
        show.restart()
    }

    Column {
        id: column

        anchors.horizontalCenter: parent.horizontalCenter
        spacing: Theme.menuSpacing

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("LEVEL %1").arg(banner.level)
            color: Theme.bannerColor
            font.family: Theme.displayFont.name
            font.pixelSize: Theme.bannerFontSize
            font.letterSpacing: 6
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: banner.note
            color: Theme.bannerNoteColor
            font.family: Theme.bodyFont.name
            font.pixelSize: Theme.bannerNoteFontSize
            font.letterSpacing: 2
        }
    }

    //in quickly enough to be noticed mid-run, out slowly so it does not snatch
    //attention back from the ship
    SequentialAnimation {
        id: show

        NumberAnimation {
            target: banner
            property: "opacity"
            to: 1
            duration: Theme.bannerInMs
            easing.type: Easing.OutQuad
        }
        PauseAnimation {
            duration: Theme.bannerHoldMs
        }
        NumberAnimation {
            target: banner
            property: "opacity"
            to: 0
            duration: Theme.bannerOutMs
            easing.type: Easing.InQuad
        }
    }
}
