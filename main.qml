import QtQuick

Window {
    width: Screen.width
    height: Screen.height
    visible: true
    title: qsTr("Hello World")
    Component.onCompleted: {
            control.setBoundaries(width, height)
        }

        onWidthChanged:control.setBoundaries(width, height)


        onHeightChanged:control.setBoundaries(width, height)

    Rectangle{
        id : move1
        width: 50
        height: 50
        color: "red"
        x: control.x //50
        y: control.y //50
        focus: true
        Keys.onPressed: (event) =>
                        {
                            if(event.key === Qt.Key_Left)
                            {
                                control.moveLeft()
                            }
                            if(event.key === Qt.Key_Right)
                            {
                                control.moveRight()
                            }
                            if(event.key === Qt.Key_Up)
                            {
                                control.applyThrust();
                            }
                            if(event.key === Qt.Key_Space)
                            {
                                control.fireBullet();
                            }
                        }
        Keys.onReleased: (event) =>
                         {
                            if(event.key === Qt.Key_Left || event.key === Qt.Key_Right)
                             {
                                control.stopMovement();
                             }
                         }
    }
    Text{
        id: scoreBoard
        text: "Score:"+ control.showScore()
        x:50
        y:50

        Connections
        {
            target: control
            function onScoreChanged()
            {
                scoreBoard.text = "Score:" + control.showScore();
            }
        }
    }
    Text{
        id: gameOverText
        text: "GAME OVER"
        visible: control.gameOver
        color: "red"
        font.pixelSize: 64
        anchors.centerIn: parent
    }
    Repeater {
        model: control.bullets
        delegate: Bullet {}
    }
    Repeater {
        model: control.enemies
        delegate: Enemy {}
    }

 }