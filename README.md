# Game01

A 2D arcade-style game built with C++ and Qt Quick (QML), demonstrating embedded-style game-loop architecture, C++/QML data binding, and object lifecycle management on the Qt Quick scene graph.

## Overview

Game01 is a small side-scrolling shooter where a player-controlled entity moves horizontally, applies upward thrust against gravity, and fires projectiles at enemies spawned at randomized intervals. The project is structured to separate simulation logic (C++) from presentation (QML), using Qt's property and signal/slot system as the binding layer between the two.

## Architecture

The build is split into three targets, one per layer:

| Target | Location | Responsibility |
|--------|----------|----------------|
| `game_core` | `src/core/` | Simulation: game loop, physics, collision, entity lifetimes. No UI or QML file knowledge. |
| `game_ui` | `src/ui/` | The `Game01` QML module: window, views, HUD, theme. No game state. |
| `appGame01` | `src/app/` | Thin executable that constructs the controller, exposes it to the engine, and loads the QML module. |

### Simulation (`src/core`)

- **`Controller`** — Central game-state owner. Drives the fixed-timestep update loop (`QTimer`, ~60 FPS), tracks player position and physics (gravity, thrust), resolves bullet/enemy collisions through a uniform grid, and manages the lifetime of `Bullet` and `Enemy` instances via `QQmlListProperty`.
- **`Bullet`** — A fired projectile with independent position updates and a destruction signal consumed by the `Controller` for cleanup.
- **`Enemy`** — A spawned enemy entity with its own position state, instantiated on a jittered timer interval.
- **`GameConfig.h`** — Every simulation tunable (entity sizes, speeds, timings, scoring) in one header.

### Presentation (`src/ui`)

`Main.qml` is the only file that touches the C++ side. It reads the `control` context property and feeds plain values down into view components, so the components below it stay reusable and testable in isolation:

- **`Main.qml`** — Root window, keyboard input forwarding, and all binding to `control`.
- **`Player.qml` / `BulletView.qml` / `EnemyView.qml`** — Passive views, positioned by their parent.
- **`Hud.qml`** — Score readout, takes the score as a property.
- **`GameOverOverlay.qml`** — End-of-run message.
- **`Theme.qml`** — Singleton holding colors and sizes, so no view hard-codes its own look.

This separation keeps simulation state and timing in C++, where memory layout and update cost are predictable, while leaving rendering and input handling declarative in QML.

> Entity sizes appear in both `src/core/GameConfig.h` and `src/ui/Theme.qml` — the simulation needs them for collision, the view needs them for drawing. Keep the two in sync.

## Controls

| Key | Action |
|-----|--------|
| Left Arrow | Move left |
| Right Arrow | Move right |
| Up Arrow | Apply thrust |
| Space | Fire bullet |

## Requirements

- Qt 6.5 or later (Qt Quick + Linguist tools)
- CMake 3.21 or later
- A C++17-capable compiler

## Build

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.y/<platform>
cmake --build build
```

Run the resulting binary from `build/src/app/` (`appGame01.app` on macOS).

To check the QML for errors without running it:

```bash
cmake --build build --target all_qmllint
```

## Project Layout

```
CMakeLists.txt          Top-level build, finds Qt and adds the three targets
src/
  core/                 Simulation library (game_core)
    GameConfig.h        Shared tunables: sizes, speeds, timings, scoring
    Controller.h/.cpp   Game loop, physics, collision, entity lifetimes
    Bullet.h/.cpp       Projectile entity
    Enemy.h/.cpp        Enemy entity
  ui/                   QML module "Game01" (game_ui)
    Main.qml            Root window, input, and the only binding to C++
    Theme.qml           Singleton: colors and sizes
    Player.qml          Player view
    BulletView.qml      Projectile view delegate
    EnemyView.qml       Enemy view delegate
    Hud.qml             Score readout
    GameOverOverlay.qml End-of-run message
  app/
    main.cpp            Entry point, engine setup, C++/QML bridge
i18n/
  Game01_en_IN.ts       Translation source, compiled into the binary at :/i18n
```

## Notes

This is an active learning project exploring Qt/QML application structure, C++ backend integration with QML via `Q_PROPERTY` and `Q_INVOKABLE`, and manual object pooling patterns relevant to resource-constrained and embedded UI development.
