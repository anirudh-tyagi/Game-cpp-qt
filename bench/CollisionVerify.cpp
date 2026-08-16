// Correctness check for the uniform-grid broad phase.
//
// A speedup only counts if the fast path finds the same collisions. This runs
// the grid and an exhaustive sweep over the same randomized scenes and compares
// the full set of overlapping (bullet, enemy) pairs.
//
// The "destroyed" dedup that Controller::checkCollision() applies is switched
// off on both sides here. With it on, the two passes visit enemies in different
// orders, so a bullet can legitimately claim a different -- but equally valid --
// enemy, and the hit counts drift apart without either side being wrong. Off,
// any difference at all is a real miss.
//
// Exits non-zero on the first sign of a mismatch, so it works as a test.

#include "core/GameConfig.h"

#include <QHash>
#include <QList>
#include <QPair>
#include <QRandomGenerator>
#include <QSet>

#include <cmath>
#include <cstdio>
#include <utility>

using namespace GameConfig;

namespace {

struct Ent { double x; double y; int id; };

int cellIndex(double value){
    return static_cast<int>(std::floor(value / cellSize));
}
qint64 cellKey(int cx, int cy){
    return (static_cast<qint64>(cx) << 32) | static_cast<quint32>(cy);
}

bool overlaps(const Ent* bullet, const Ent* enemy){
    return bullet->x + bulletWidth > enemy->x
        && bullet->x < enemy->x + enemyWidth
        && bullet->y < enemy->y + enemyHeight
        && bullet->y + bulletHeight > enemy->y;
}

constexpr double fieldWidth = 1280.0;
constexpr double fieldHeight = 720.0;
constexpr int trials = 2000;

} // namespace

int main()
{
    QRandomGenerator rng(99);
    int mismatches = 0;
    qint64 pairsChecked = 0;

    for(int trial = 0; trial < trials; trial++){
        //vary the density too, a grid that only works when crowded is no good
        const int bulletCount = 1 + rng.bounded(60);
        const int enemyCount = 1 + rng.bounded(120);

        QList<Ent*> bullets;
        QList<Ent*> enemies;
        for(int i = 0; i < bulletCount; i++){
            bullets.append(new Ent{rng.bounded(fieldWidth), rng.bounded(fieldHeight), i});
        }
        for(int i = 0; i < enemyCount; i++){
            enemies.append(new Ent{rng.bounded(fieldWidth), rng.bounded(fieldHeight), i});
        }

        QSet<QPair<int, int>> viaGrid;
        QSet<QPair<int, int>> viaSweep;

        QHash<qint64, QList<Ent*>> grid;
        grid.reserve(enemies.size() * 4);
        for(Ent* enemy : std::as_const(enemies)){
            for(int cx = cellIndex(enemy->x); cx <= cellIndex(enemy->x + enemyWidth); cx++){
                for(int cy = cellIndex(enemy->y); cy <= cellIndex(enemy->y + enemyHeight); cy++){
                    grid[cellKey(cx, cy)].append(enemy);
                }
            }
        }

        for(Ent* bullet : std::as_const(bullets)){
            for(int cx = cellIndex(bullet->x); cx <= cellIndex(bullet->x + bulletWidth); cx++){
                for(int cy = cellIndex(bullet->y); cy <= cellIndex(bullet->y + bulletHeight); cy++){
                    const auto cell = grid.constFind(cellKey(cx, cy));
                    if(cell == grid.constEnd()){
                        continue;
                    }
                    for(Ent* enemy : *cell){
                        if(overlaps(bullet, enemy)){
                            viaGrid.insert({bullet->id, enemy->id});
                        }
                    }
                }
            }
        }

        for(Ent* bullet : std::as_const(bullets)){
            for(Ent* enemy : std::as_const(enemies)){
                if(overlaps(bullet, enemy)){
                    viaSweep.insert({bullet->id, enemy->id});
                }
            }
        }

        pairsChecked += viaSweep.size();

        if(viaGrid != viaSweep){
            mismatches++;
            printf("MISMATCH trial %d (%d bullets, %d enemies): grid found %lld, sweep found %lld\n",
                   trial, bulletCount, enemyCount,
                   static_cast<long long>(viaGrid.size()),
                   static_cast<long long>(viaSweep.size()));
        }

        qDeleteAll(bullets);
        qDeleteAll(enemies);
    }

    if(mismatches){
        printf("\nFAILED: %d of %d trials disagreed\n", mismatches, trials);
        return 1;
    }

    printf("OK: %d/%d trials produced identical pair sets (%lld overlapping pairs checked)\n",
           trials, trials, static_cast<long long>(pairsChecked));
    return 0;
}
