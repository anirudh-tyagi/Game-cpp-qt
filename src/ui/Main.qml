import QtQuick

//Root window, and the only file that talks to the C++ "control" object.
//Everything below it is a plain view component driven by properties.
Window {
    id: root

    width: Screen.width
    height: Screen.height
    visible: true
    title: qsTr("Skyward")
    color: Theme.background

    //the simulation needs the window size to clamp the player and to know
    //where enemies leave the screen
    Component.onCompleted: control.setBoundaries(width, height)
    onWidthChanged: control.setBoundaries(width, height)
    onHeightChanged: control.setBoundaries(width, height)

    //night sky, always on. It sits under the menu too, so the start page and
    //the run share one backdrop instead of cutting between two
    Starfield {
        anchors.fill: parent
    }

    //keyboard goes straight to the controller, no game state lives in QML.
    //the controller ignores the play keys while no run is in progress
    Item {
        id: input

        anchors.fill: parent
        focus: true

        //which arrows are physically down. The controller only carries one
        //direction, so releasing one arrow while the other is still held has to
        //hand over rather than stop
        property bool leftHeld: false
        property bool rightHeld: false

        Keys.onPressed: (event) => {
            //auto-repeat would drive the ship at the OS key-repeat rate on top
            //of the controller's own 60fps timer, making both movement speed and
            //fire rate depend on a keyboard setting
            if (event.isAutoRepeat)
                return

            if (event.key === Qt.Key_Left) {
                input.leftHeld = true
                control.moveLeft()
            }
            if (event.key === Qt.Key_Right) {
                input.rightHeld = true
                control.moveRight()
            }
            if (event.key === Qt.Key_Up)
                control.applyThrust()
            if (event.key === Qt.Key_Space)
                control.fireBullet()

            if (event.key === Qt.Key_Escape) {
                //escape means "stop" during a run and "leave" on the menu
                if (control.running)
                    control.togglePause()
                else
                    Qt.quit()
            }

            //start, restart, or resume, whichever page is up
            if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                if (control.paused)
                    control.togglePause()
                else if (!control.running)
                    control.startGame()
            }
        }

        Keys.onReleased: (event) => {
            //X11 synthesises press/release pairs for auto-repeat, which would
            //otherwise stutter a held arrow
            if (event.isAutoRepeat)
                return

            if (event.key === Qt.Key_Left)
                input.leftHeld = false
            if (event.key === Qt.Key_Right)
                input.rightHeld = false

            if (event.key === Qt.Key_Left || event.key === Qt.Key_Right) {
                //only actually stop once neither arrow is down
                if (input.leftHeld)
                    control.moveLeft()
                else if (input.rightHeld)
                    control.moveRight()
                else
                    control.stopMovement()
            }
        }

        //a run frozen mid-press resumes with the keys in an unknown state, and
        //the controller drops its direction on resume, so drop ours too
        Connections {
            target: control

            function onPausedChanged() {
                input.leftHeld = false
                input.rightHeld = false
            }
        }
    }

    //the play field, hidden until the first run so the start page stands alone
    Item {
        anchors.fill: parent
        visible: control.running || control.gameOver

        Player {
            x: control.x
            y: control.y
            thrusting: control.thrusting
        }

        //modelData is the Bullet/Enemy object itself, the lists are
        //QQmlListProperty of QObjects
        Repeater {
            model: control.bullets
            delegate: BulletView {
                required property var modelData
                x: modelData.x
                y: modelData.y
            }
        }

        Repeater {
            model: control.enemies
            delegate: EnemyView {
                required property var modelData
                x: modelData.x
                y: modelData.y
                sprite: modelData.sprite
            }
        }

        Hud {
            x: Theme.hudMargin
            y: Theme.hudMargin
            score: control.score
            level: control.level
        }

        //sits above the play field, high enough not to cover the ship
        LevelBanner {
            id: levelBanner

            anchors.horizontalCenter: parent.horizontalCenter
            y: parent.height * 0.28
            width: parent.width
            level: control.level
            note: qsTr("they come faster now")
        }

        //the level also drops back to 1 on a restart, and that is not something
        //to announce
        Connections {
            target: control

            function onLevelChanged() {
                if (control.running && control.level > 1)
                    levelBanner.announce()
            }
        }
    }

    //one page for all three moments, the wording is all that changes
    MenuOverlay {
        anchors.fill: parent
        visible: !control.running || control.paused

        title: {
            if (control.paused)
                return qsTr("Held")
            return control.gameOver ? qsTr("Take a breath") : qsTr("Skyward")
        }
        subtitle: {
            if (control.paused)
                return qsTr("Still going. You are on %1, at level %2.")
                       .arg(control.score).arg(control.level)
            if (control.gameOver)
                return qsTr("That run is over. You scored %1, and reached level %2.")
                       .arg(control.score).arg(control.level)
            return qsTr("A gentle climb. Drift, aim, and take your time.")
        }
        hint: control.paused
              ? qsTr("esc or enter to carry on")
              : qsTr("← → to move · ↑ to thrust · space to fire · esc to pause")
        actionText: {
            if (control.paused)
                return qsTr("Resume")
            return control.gameOver ? qsTr("Play again") : qsTr("Start game")
        }

        //resuming and starting are different things, a pause must not wipe the run
        onActionClicked: control.paused ? control.togglePause() : control.startGame()
        onQuitClicked: Qt.quit()
    }
}
