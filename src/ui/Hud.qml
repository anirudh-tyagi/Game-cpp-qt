import QtQuick

//Score readout. Takes the value as a property so it stays independent of
//where the score comes from.
Text {
    id: hud

    property real score: 0

    text: qsTr("Score: %1").arg(hud.score)
    color: Theme.scoreColor
    font.pixelSize: Theme.scoreFontSize
}
