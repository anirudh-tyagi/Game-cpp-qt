pragma Singleton

import QtQuick

//Single place for the look of the game. The entity sizes mirror
//src/core/GameConfig.h, keep the two in sync when changing anything here.
QtObject {
    id: theme

    //Doto ships in assets/ and is registered from assets.qrc. Held here so
    //every screen pulls the same family off one load
    readonly property FontLoader displayFont: FontLoader {
        source: "qrc:/fonts/Doto-ExtraBold.ttf"
    }
    readonly property FontLoader bodyFont: FontLoader {
        source: "qrc:/fonts/Doto-Bold.ttf"
    }

    //deep, low-contrast night rather than pure black, which would make the
    //sprites look cut out
    readonly property color background: "#0C1018"
    readonly property color backgroundGlow: "#161E2C" //horizon wash at the bottom
    readonly property color starColor: "#C9D6E8"
    readonly property color scoreColor: "#C2CFE0"
    readonly property color levelColor: "#6F8199" //secondary to the score

    //sprites, one per entity. Sizes stay in step with GameConfig.h
    readonly property string playerSource: "qrc:/player/rocket.png"
    readonly property string thrusterSource: "qrc:/player/thruster.gif"
    readonly property string bulletSource: "qrc:/bullet/bullet.png"
    //enemy sprites are enemy1..enemy5, the Enemy object carries which one
    function enemySource(sprite) {
        return "qrc:/enemy/enemy" + sprite + ".png"
    }

    readonly property real playerSize: 50
    readonly property real enemySize: 50
    readonly property real bulletWidth: 10
    readonly property real bulletHeight: 30
    //drawn larger than the 50x50 collision box, the plume is decoration and
    //must not suggest a bigger hitbox than the ship actually has
    readonly property real thrusterWidth: 26
    readonly property real thrusterHeight: 40

    readonly property int starCount: 90

    readonly property int scoreFontSize: 20
    readonly property int levelFontSize: 15
    readonly property real hudMargin: 50

    //level-up announcement, deliberately dimmer than the menu title so it reads
    //as passing information rather than as a screen taking over
    readonly property color bannerColor: "#B9CBDE"
    readonly property color bannerNoteColor: "#6F8199"
    readonly property int bannerFontSize: 46
    readonly property int bannerNoteFontSize: 15
    readonly property int bannerInMs: 260
    readonly property int bannerHoldMs: 900
    readonly property int bannerOutMs: 700

    //menu, the same night palette lifted just enough to read as a surface
    readonly property color menuVeilTop: Qt.rgba(0.047, 0.063, 0.094, 0.86)
    readonly property color menuVeilBottom: Qt.rgba(0.035, 0.047, 0.075, 0.96)
    readonly property color menuPanel: "#161C28"
    readonly property color menuPanelBorder: "#2A3446"
    readonly property color menuTitleColor: "#E6EDF7"
    readonly property color menuSubtitleColor: "#8B99AE"
    readonly property color menuHintColor: "#5D6B80"

    //muted teal, the one warm-ish accent in an otherwise cool screen
    readonly property color buttonColor: "#243543"
    readonly property color buttonHoverColor: "#2E4553"
    readonly property color buttonPressedColor: "#1D2B36"
    readonly property color buttonTextColor: "#D6E4EE"
    //quit is the quieter of the two, so it recedes instead of competing
    readonly property color buttonQuietColor: "#1A2130"
    readonly property color buttonQuietHoverColor: "#222B3C"
    readonly property color buttonQuietPressedColor: "#151B27"
    readonly property color buttonQuietTextColor: "#93A1B5"

    readonly property int menuTitleFontSize: 52
    readonly property int menuSubtitleFontSize: 18
    readonly property int menuHintFontSize: 14
    readonly property int buttonFontSize: 19

    readonly property real menuPanelWidth: 440
    readonly property real menuPanelPadding: 48
    readonly property real menuSpacing: 14
    readonly property real menuRadius: 22
    readonly property real buttonWidth: 280
    readonly property real buttonHeight: 54
    readonly property real buttonRadius: 27

    //slow on purpose, quick transitions read as urgent
    readonly property int menuFadeMs: 420
    readonly property int buttonFadeMs: 160
}
