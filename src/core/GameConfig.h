#ifndef GAMECONFIG_H
#define GAMECONFIG_H

//Tunables shared by the simulation. The view mirrors the sizes in
//src/ui/Theme.qml, keep the two in sync when changing anything here.
namespace GameConfig {

//entity boxes, used for spawning and for collision
constexpr double playerWidth = 50.0;
constexpr double playerHeight = 50.0;
constexpr double enemyWidth = 50.0;
constexpr double enemyHeight = 50.0;
constexpr double bulletWidth = 10.0;
constexpr double bulletHeight = 30.0;

//timing
constexpr int frameIntervalMs = 16; //~60fps
constexpr int enemyFrameIntervalMs = 50;
constexpr int enemySpawnMinMs = 1000;
constexpr int enemySpawnJitterMs = 2000; //spawn lands in [min, min + jitter)

//motion
constexpr double playerXSpeed = 10.0;
constexpr double bulletYSpeed = -10.0; //negative is up the screen
constexpr double enemyYSpeed = 1.0;
constexpr double gravity = 0.5;
constexpr double maxThrust = -15.0;

//collision grid cell, at least as big as the largest entity so a box never
//spans more than 2x2 cells
constexpr double cellSize = 64.0;

//scoring
constexpr double pointsPerKill = 10.0;

} // namespace GameConfig

#endif // GAMECONFIG_H
