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

//levels. The level is derived from the score rather than counted separately,
//so the two can never drift apart
constexpr double pointsPerLevel = 100.0; //10 kills per level

//difficulty ramp, two knobs: enemies fall faster and arrive closer together.
//both are clamped, an uncapped ramp turns unplayable within a few levels
constexpr double enemySpeedPerLevel = 0.35;
//an enemy moves this far per enemyFrameIntervalMs tick, so the cap also keeps a
//falling enemy from stepping clean over the player box between two frames
constexpr double enemyMaxYSpeed = 6.0;
constexpr double spawnScalePerLevel = 0.85; //compounding, so ~15% tighter each level
constexpr int enemySpawnFloorMs = 320;
constexpr int enemySpawnJitterFloorMs = 400;

} // namespace GameConfig

#endif // GAMECONFIG_H
