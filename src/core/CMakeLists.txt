# Simulation layer: game loop, physics, entity lifetimes.
# Knows nothing about QML files or the scene graph, it only exposes state
# through Q_PROPERTY / Q_INVOKABLE so a view layer can bind to it.
qt_add_library(game_core STATIC
    GameConfig.h
    Controller.h
    Controller.cpp
    Bullet.h
    Bullet.cpp
    Enemy.h
    Enemy.cpp
)

# headers are included as "core/Controller.h", so src/ is the include root
target_include_directories(game_core PUBLIC ${CMAKE_SOURCE_DIR}/src)

# Qml, not Quick: QQmlListProperty is the only QML-side type used here
target_link_libraries(game_core PUBLIC Qt6::Qml)
