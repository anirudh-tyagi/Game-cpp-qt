// Uniform-grid broad phase vs an exhaustive O(n*m) sweep.
//
// The grid pass below mirrors Controller::checkCollision() in
// src/core/Controller.cpp: same cell size, same packed qint64 cell key, same
// QHash/QSet/QList containers, same AABB overlap test, same "destroyed" dedup,
// same reverse iteration over the bullet list. Entity sizes come from
// core/GameConfig.h rather than being restated, so retuning the game retunes
// the benchmark with it.
//
// What this does NOT measure: QObject construction, signal emission, or the
// QML delegate rebuild that a real frame also pays for. Entities here are plain
// structs, so the numbers isolate the collision pass itself.

#include "core/GameConfig.h"

#include <QElapsedTimer>
#include <QHash>
#include <QList>
#include <QRandomGenerator>
#include <QSet>

#include <cmath>
#include <cstdio>
#include <utility>

using namespace GameConfig;

namespace {

struct Ent { double x; double y; };

//identical to the helpers in Controller.cpp
int cellIndex(double value){
    return static_cast<int>(std::floor(value / cellSize));
}
qint64 cellKey(int cx, int cy){
    return (static_cast<qint64>(cx) << 32) | static_cast<quint32>(cy);
}

//the shipped algorithm
int gridPass(const QList<Ent*>& bulletList, const QList<Ent*>& enemyList,
             quint64& pairTests)
{
    if(bulletList.isEmpty() || enemyList.isEmpty()){
        return 0;
    }

    QHash<qint64, QList<Ent*>> grid;
    grid.reserve(enemyList.size() * 4);

    for(Ent* enemy : std::as_const(enemyList)){
        const int minCX = cellIndex(enemy->x);
        const int maxCX = cellIndex(enemy->x + enemyWidth);
        const int minCY = cellIndex(enemy->y);
        const int maxCY = cellIndex(enemy->y + enemyHeight);

        for(int cx = minCX; cx <= maxCX; cx++){
            for(int cy = minCY; cy <= maxCY; cy++){
                grid[cellKey(cx, cy)].append(enemy);
            }
        }
    }

    QSet<Ent*> destroyed;
    int hits = 0;

    for(int i = bulletList.size() - 1; i >= 0; i--){
        Ent* bullet = bulletList[i];

        const double bulletLeft = bullet->x;
        const double bulletRight = bulletLeft + bulletWidth;
        const double bulletTop = bullet->y;
        const double bulletBottom = bulletTop + bulletHeight;

        const int minCX = cellIndex(bulletLeft);
        const int maxCX = cellIndex(bulletRight);
        const int minCY = cellIndex(bulletTop);
        const int maxCY = cellIndex(bulletBottom);

        Ent* hit = nullptr;

        for(int cx = minCX; cx <= maxCX && !hit; cx++){
            for(int cy = minCY; cy <= maxCY && !hit; cy++){
                const auto cell = grid.constFind(cellKey(cx, cy));
                if(cell == grid.constEnd()){
                    continue;
                }
                for(Ent* enemy : *cell){
                    if(destroyed.contains(enemy)){
                        continue;
                    }
                    pairTests++;
                    const double enemyLeft = enemy->x;
                    const double enemyRight = enemyLeft + enemyWidth;
                    const double enemyTop = enemy->y;
                    const double enemyBottom = enemyTop + enemyHeight;

                    if(bulletRight > enemyLeft && bulletLeft < enemyRight
                        && bulletTop < enemyBottom && bulletBottom > enemyTop)
                    {
                        hit = enemy;
                        break;
                    }
                }
            }
        }

        if(hit){
            destroyed.insert(hit);
            hits++;
        }
    }
    return hits;
}

//the baseline it is measured against: every bullet against every enemy
int naivePass(const QList<Ent*>& bulletList, const QList<Ent*>& enemyList,
              quint64& pairTests)
{
    if(bulletList.isEmpty() || enemyList.isEmpty()){
        return 0;
    }

    QSet<Ent*> destroyed;
    int hits = 0;

    for(int i = bulletList.size() - 1; i >= 0; i--){
        Ent* bullet = bulletList[i];

        const double bulletLeft = bullet->x;
        const double bulletRight = bulletLeft + bulletWidth;
        const double bulletTop = bullet->y;
        const double bulletBottom = bulletTop + bulletHeight;

        Ent* hit = nullptr;

        for(Ent* enemy : std::as_const(enemyList)){
            if(destroyed.contains(enemy)){
                continue;
            }
            pairTests++;
            const double enemyLeft = enemy->x;
            const double enemyRight = enemyLeft + enemyWidth;
            const double enemyTop = enemy->y;
            const double enemyBottom = enemyTop + enemyHeight;

            if(bulletRight > enemyLeft && bulletLeft < enemyRight
                && bulletTop < enemyBottom && bulletBottom > enemyTop)
            {
                hit = enemy;
                break;
            }
        }

        if(hit){
            destroyed.insert(hit);
            hits++;
        }
    }
    return hits;
}

//a play field the size of a typical window
constexpr double fieldWidth = 1280.0;
constexpr double fieldHeight = 720.0;

//fixed seeds, so a re-run compares against the same scene
void fill(QList<Ent*>& list, int count, quint32 seed){
    QRandomGenerator rng(seed);
    for(int i = 0; i < count; i++){
        list.append(new Ent{rng.bounded(fieldWidth - enemyWidth),
                            rng.bounded(fieldHeight - enemyHeight)});
    }
}

void run(int bulletCount, int enemyCount, int iterations, const char* note)
{
    QList<Ent*> bullets;
    QList<Ent*> enemies;
    fill(bullets, bulletCount, 1234);
    fill(enemies, enemyCount, 5678);

    quint64 gridTests = 0;
    quint64 naiveTests = 0;
    QElapsedTimer timer;

    //warm the caches and let the branch predictor settle
    for(int i = 0; i < 50; i++){
        quint64 discard = 0;
        gridPass(bullets, enemies, discard);
        naivePass(bullets, enemies, discard);
    }

    timer.start();
    for(int i = 0; i < iterations; i++){
        gridPass(bullets, enemies, gridTests);
    }
    const double gridUs = double(timer.nsecsElapsed()) / iterations / 1000.0;

    timer.start();
    for(int i = 0; i < iterations; i++){
        naivePass(bullets, enemies, naiveTests);
    }
    const double naiveUs = double(timer.nsecsElapsed()) / iterations / 1000.0;

    const double gridPerFrame = double(gridTests) / iterations;
    const double naivePerFrame = double(naiveTests) / iterations;

    printf("%5d x %5d | %9.1f | %9.1f | %6.1fx | %8.0f | %9.0f | %5.1fx | %s\n",
           bulletCount, enemyCount, gridUs, naiveUs, naiveUs / gridUs,
           gridPerFrame, naivePerFrame, naivePerFrame / gridPerFrame, note);

    qDeleteAll(bullets);
    qDeleteAll(enemies);
}

} // namespace

int main()
{
    printf("Uniform-grid broad phase vs exhaustive sweep\n");
    printf("cell size %.0f, enemy %.0fx%.0f, bullet %.0fx%.0f, field %.0fx%.0f\n",
           cellSize, enemyWidth, enemyHeight, bulletWidth, bulletHeight,
           fieldWidth, fieldHeight);
    printf("frame budget at %d ms tick: %d us\n\n",
           frameIntervalMs, frameIntervalMs * 1000);

    printf("bullets x enemies | grid (us) | naive (us) | speedup | grid AABB | naive AABB | fewer | note\n");
    printf("------------------|-----------|------------|---------|-----------|------------|-------|-----\n");

    run(10,    12, 50000, "typical live play");
    run(15,    25, 50000, "");
    run(30,    60, 30000, "");
    run(50,   100, 10000, "");
    run(100,  250,  5000, "sweep still ahead");
    run(200,  500,  2000, "grid pulls ahead");
    run(500, 1000,  1000, "");
    run(1000, 2000,   400, "stress");

    return 0;
}
