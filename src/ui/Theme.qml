pragma Singleton

import QtQuick

//Single place for the look of the game. The entity sizes mirror
//src/core/GameConfig.h, keep the two in sync when changing anything here.
QtObject {
    readonly property color background: "white"
    readonly property color playerColor: "red"
    readonly property color enemyColor: "blue"
    readonly property color bulletColor: "black"
    readonly property color scoreColor: "black"
    readonly property color gameOverColor: "red"

    readonly property real playerSize: 50
    readonly property real enemySize: 50
    readonly property real bulletWidth: 10
    readonly property real bulletHeight: 30

    readonly property int scoreFontSize: 16
    readonly property int gameOverFontSize: 64
    readonly property real hudMargin: 50
}
