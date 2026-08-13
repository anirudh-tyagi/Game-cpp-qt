# Game01

A 2D arcade-style game built with C++ and Qt Quick (QML), demonstrating embedded-style game-loop architecture, C++/QML data binding, and object lifecycle management on the Qt Quick scene graph.

## Overview

Game01 is a small side-scrolling shooter where a player-controlled entity moves horizontally, applies upward thrust against gravity, and fires projectiles at enemies spawned at randomized intervals. The project is structured to separate simulation logic (C++) from presentation (QML), using Qt's property and signal/slot system as the binding layer between the two.

## Architecture

- **`Controller`** — Central game-state owner. Drives the fixed-timestep update loop (`QTimer`, ~60 FPS), tracks player position and physics (gravity, thrust), and manages the lifetime of `Bullet` and `Enemy` instances via `QQmlListProperty`.
- **`Bullet`** — Represents a fired projectile with independent position updates and a destruction signal consumed by the `Controller` for cleanup.
- **`Enemy`** — Represents a spawned enemy entity with its own position state, randomly instantiated on a jittered timer interval.
- **QML front end** (`main.qml`, `Bullet.qml`, `Enemy.qml`) — Declarative view layer bound to the C++ model via context properties and `Repeater` delegates; keyboard input is forwarded from QML to `Controller` invokable methods.

This separation keeps simulation state and timing in C++, where memory layout and update cost are predictable, while leaving rendering and input handling declarative in QML.

## Controls

| Key | Action |
|-----|--------|
| Left Arrow | Move left |
| Right Arrow | Move right |
| Up Arrow | Apply thrust |
| Space | Fire bullet |

## Requirements

- Qt 6.2 or later (Qt Quick module)
- CMake 3.16 or later
- A C++17-capable compiler

## Build

```bash
cmake -S . -B build
cmake --build build
```

Run the resulting `appGame01` binary from the build output directory (or launch `appGame01.app` on macOS).

## Project Layout

```
main.cpp / main.qml   Application entry point and root window
Controller.h / .cpp   Game loop, physics, and entity lifecycle management
Bullet.h / .cpp       Projectile entity
Bullet.qml            Projectile view delegate
Enemy.h / .cpp        Enemy entity
Enemy.qml             Enemy view delegate
CMakeLists.txt        Build configuration (qt_add_qml_module)
```

## Notes

This is an active learning project exploring Qt/QML application structure, C++ backend integration with QML via `Q_PROPERTY` and `Q_INVOKABLE`, and manual object pooling patterns relevant to resource-constrained and embedded UI development.
