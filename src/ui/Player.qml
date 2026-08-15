import QtQuick

//The player ship. A pure view, its position and thrust state are driven from
//outside. The item stays exactly the collision box from GameConfig.h, the
//exhaust hangs off the bottom of it without widening it.

Item {
    id: player

    property bool thrusting: false

    width: Theme.playerSize
    height: Theme.playerSize

    //behind the hull and anchored to its base, so it reads as coming out of
    //the engine rather than sitting next to it
    AnimatedImage {
        id: plume

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.bottom
        anchors.topMargin: -6
        width: Theme.thrusterWidth
        height: Theme.thrusterHeight
        source: Theme.thrusterSource
        fillMode: Image.PreserveAspectFit
        //only burning frames while the ship is climbing
        playing: player.thrusting
        opacity: player.thrusting ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: 120
                easing.type: Easing.OutQuad
            }
        }
    }

    Image {
        anchors.fill: parent
        source: Theme.playerSource
        fillMode: Image.PreserveAspectFit
        //the source is far larger than 50px, so let the scene graph downscale
        //it once at load instead of every frame
        sourceSize.width: Theme.playerSize * 2
        smooth: true
    }
}
