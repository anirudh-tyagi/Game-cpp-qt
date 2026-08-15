import QtQuick

//Delegate for one Enemy in the controller's enemy list. Which of the five
//sprites it wears comes from the Enemy object, not from here, so it stays put
//when the view rebuilds.
Item {
    id: enemyView

    property int sprite: 1

    width: Theme.enemySize
    height: Theme.enemySize

    Image {
        anchors.fill: parent
        source: Theme.enemySource(enemyView.sprite)
        fillMode: Image.PreserveAspectFit
        sourceSize.width: Theme.enemySize * 2
        smooth: true
    }

    //a slow drift on the way down, each sprite offset by its own variant so the
    //field never looks like it is marching in step
    RotationAnimation on rotation {
        loops: Animation.Infinite
        from: -6
        to: 6
        duration: 2600 + (enemyView.sprite * 220)
        easing.type: Easing.InOutSine
    }
}
