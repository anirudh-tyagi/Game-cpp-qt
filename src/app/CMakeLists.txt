# Application entry point: wires the simulation (game_core) to the QML module
# (game_ui) and starts the engine. It owns no game logic and no UI of its own.

# sprites and fonts, compiled in here rather than into the static game_ui so the
# resource initialiser is guaranteed to survive the link. QML reaches them as
# qrc:/enemy/..., qrc:/player/..., qrc:/bullet/... and qrc:/fonts/...
qt_add_resources(GAME_ASSETS ${CMAKE_SOURCE_DIR}/assets/assets.qrc)

qt_add_executable(appGame01
    main.cpp
    ${GAME_ASSETS}
)

# game_uiplugin comes with qt_add_qml_module and registers the QML module,
# linking it is what makes loadFromModule("Game01", "Main") resolve
target_link_libraries(appGame01 PRIVATE
    Qt6::Quick
    game_core
    game_uiplugin
)

# compiles i18n/*.ts into the binary at :/i18n, where main.cpp looks for them
qt_add_translations(appGame01
    TS_FILES ${CMAKE_SOURCE_DIR}/i18n/Game01_en_IN.ts
)

set_target_properties(appGame01 PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appGame01
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
