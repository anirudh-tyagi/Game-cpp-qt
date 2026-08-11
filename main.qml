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
        x: 100
        y: 100
        focus: true
        Keys.onPressed: (event) =>
                        {
                            if(event.key === Qt.Key_Left)
                            {
                                move1.x -= 10;
                            }
                            if(event.key === Qt.Key_Right)
                            {
                                move1.x += 10;
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