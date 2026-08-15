import QtQuick

//Score and level readout. Takes both as properties so it stays independent of
//where they come from.
Column {
    id: hud

    property real score: 0
    property int level: 1

    spacing: 4

    Text {
        text: qsTr("Score: %1").arg(hud.score)
        color: Theme.scoreColor
        font.family: Theme.bodyFont.name
        font.pixelSize: Theme.scoreFontSize
        font.letterSpacing: 1
    }

    Text {
        text: qsTr("Level %1").arg(hud.level)
        color: Theme.levelColor
        font.family: Theme.bodyFont.name
        font.pixelSize: Theme.levelFontSize
        font.letterSpacing: 1
    }
}
