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

    //keyboard goes straight to the controller, no game state lives in QML
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
        }

        Keys.onReleased: (event) => {
            if (event.key === Qt.Key_Left || event.key === Qt.Key_Right)
                control.stopMovement()
        }
    }

    Player {
        x: control.x
        y: control.y
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
        }
    }

    Hud {
        x: Theme.hudMargin
        y: Theme.hudMargin
        score: control.score
    }

    GameOverOverlay {
        anchors.centerIn: parent
        visible: control.gameOver
    }
}
