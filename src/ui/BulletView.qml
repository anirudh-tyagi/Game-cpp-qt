import QtQuick

//Delegate for one Bullet in the controller's bullet list.
Image {
    width: Theme.bulletWidth
    height: Theme.bulletHeight
    source: Theme.bulletSource
    fillMode: Image.PreserveAspectFit
    sourceSize.width: Theme.bulletWidth * 3
    smooth: true
}
