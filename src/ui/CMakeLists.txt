# Presentation layer, shipped as the "Game01" QML module. Paths here are
# relative to this directory, so the types land in the module root
# (Main, Player, Hud, ...) and are reachable as qrc:/qt/qml/Game01/...
qt_add_library(game_ui STATIC)

set_source_files_properties(Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(game_ui
    URI Game01
    VERSION 1.0
    # the build-dir copy has to sit at <binary dir>/Game01 for qmllint and the
    # Qt Creator QML tooling to resolve the module
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Game01
    QML_FILES
        Main.qml
        Theme.qml
        Player.qml
        BulletView.qml
        EnemyView.qml
        Hud.qml
        GameOverOverlay.qml
)

target_link_libraries(game_ui PRIVATE Qt6::Quick)
