import QtQuick

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

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
                                move1.y -= 10;
                            }
                            if(event.key === Qt.Key_Down)
                            {
                                move1.y += 10;
                            }
                        }
    }
 }