import QtQuick

//Root window, and the only file that talks to the C++ "control" object.
//Everything below it is a plain view component driven by properties.
Window {
    id: root

    width: Screen.width
    height: Screen.height
    visible: true
    title: qsTr("Game01")
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
        anchors.fill: parent
        focus: true

        Keys.onPressed: (event) => {
            if (event.key === Qt.Key_Left)
                control.moveLeft()
            if (event.key === Qt.Key_Right)
                control.moveRight()
            if (event.key === Qt.Key_Up)
                control.applyThrust()
            if (event.key === Qt.Key_Space)
                control.fireBullet()

            //menu shortcuts, only live while the menu is up
            if (!control.running) {
                if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter)
                    control.startGame()
                if (event.key === Qt.Key_Escape)
                    Qt.quit()
            }
        }

        Keys.onReleased: (event) => {
            if (event.key === Qt.Key_Left || event.key === Qt.Key_Right)
                control.stopMovement()
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

    //one page for both moments, the wording is all that changes
    MenuOverlay {
        anchors.fill: parent
        visible: !control.running

        title: control.gameOver ? qsTr("Take a breath") : qsTr("Skyward")
        subtitle: control.gameOver
                  ? qsTr("That run is over. You scored %1, and reached level %2.")
                    .arg(control.score).arg(control.level)
                  : qsTr("A gentle climb. Drift, aim, and take your time.")
        hint: qsTr("← → to move · ↑ to thrust · space to fire")
        actionText: control.gameOver ? qsTr("Play again") : qsTr("Start game")

        onActionClicked: control.startGame()
        onQuitClicked: Qt.quit()
    }
}
