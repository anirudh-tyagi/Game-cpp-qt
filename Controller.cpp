#include "Controller.h"
#include <qdebug.h>
#include <QRandomGenerator>
#include <QHash>
#include <QSet>
#include <cmath>
#include <utility>

namespace {
//cell must be at least as big as the largest entity, so a box never
//spans more than 2x2 cells
constexpr double cellSize = 64.0;
constexpr double bulletWidth = 10.0;
constexpr double bulletHeight = 30.0;
constexpr double enemySize = 50.0;

int cellIndex(double value){
    return static_cast<int>(std::floor(value / cellSize));
}

//pack the two cell coords into one key so we can use a flat QHash
qint64 cellKey(int cx, int cy){
    return (static_cast<qint64>(cx) << 32) | static_cast<quint32>(cy);
}
}

Controller::Controller(QObject* parent)
    : QObject(parent),
    m_x(50),
    m_y(50),
    xSpeed(10),
    ySpeed(10),
    minX(0),
    maxX(0),
    bottomY(0),
    moveDirection(0)
{
    connect(&time, &QTimer::timeout, this, &Controller::updateState);
    time.start(16); //60fps

    connect(&move, &QTimer::timeout, this, &Controller::updateMovement);
    move.setInterval(16); //60fps, started only while a key is held

    connect(&startE, &QTimer::timeout, this, &Controller::createEnemies);
    startE.start(1000 + QRandomGenerator::global()->bounded(2000));//1-3 seconds
}

double Controller::despawnY() const
{
    //bottomY is height - 50, so this is one enemy height below the window
    return bottomY + (2 * enemySize);
}

void Controller::setBoundaries(double width, double height)
{
    minX = 0;
    maxX = width - 50;

    bottomY = height - 50;

    // Start player at bottom
    m_y = bottomY;

    // enemies spawned before a resize would otherwise keep the old cutoff
    for(Enemy* enemy : std::as_const(enemyList)){
        enemy->setDespawnY(despawnY());
    }

    emit yChanged();
}

void Controller::moveLeft()
{
    moveDirection = -1;
    updateMovement(); //respond to the very first press without waiting a tick
    if (!move.isActive())
    {
        move.start();
    }
}

void Controller::moveRight(){
    moveDirection = 1;
    updateMovement();
    if (!move.isActive())
    {
        move.start();
    }
}

void Controller::stopMovement()
{
    moveDirection =0;
    move.stop();
}

void Controller::applyThrust(){
    ySpeed= maxThrust;
    if(m_y < bottomY/1.5){
        ySpeed = 0;
    }
}

Q_INVOKABLE void Controller::fireBullet()
{
    Bullet* newBullet = new Bullet(this, this);
    newBullet->setX(m_x + 25);
    newBullet->setY(m_y);
    bulletList.append(newBullet);
    emit bulletChanged();
}

Q_INVOKABLE void Controller::createEnemies()
{
    startE.start(1000 + QRandomGenerator::global()->bounded(2000));

    //nothing to spawn into until QML has handed us the window size
    if(maxX <= 0){
        return;
    }

    Enemy* newEnemy = new Enemy(this, this);
    newEnemy->setX(QRandomGenerator::global()->bounded(static_cast<int>(maxX) + 1));
    newEnemy->setY(-enemySize);
    newEnemy->setDespawnY(despawnY());
    enemyList.append(newEnemy);

    emit enemyChanged();
}

//slot
void Controller::updateState(){
    m_y += ySpeed;
    ySpeed += gravity;

    if(m_y > bottomY){
        m_y = bottomY;
    }
    checkCollision();
    emit yChanged();
}

void Controller::deleteBullet(Bullet *bullet)
{
    int index= bulletList.indexOf(bullet);
    if (index != -1)
    {
        delete bulletList[index];
        //deletes the memory
        bulletList.removeAt(index);
        //deletes the place inside list of the instance
        emit bulletChanged();
        //qInfo()<<"Bullet Destroyed";
    }
}


void Controller::deleteEnemy(Enemy *enemy)
{
    int index= enemyList.indexOf(enemy);
    if (index != -1)
    {
        delete enemyList[index];
        //deletes the memory
        enemyList.removeAt(index);
        //deletes the place inside list of the instance
        emit enemyChanged();
        //qInfo()<<"Enemy Destroyed";
    }
}

void Controller::checkCollision()
{
    if(bulletList.isEmpty() || enemyList.isEmpty()){
        return;
    }

    //bucket every enemy into the grid cells its box overlaps.
    //rebuilt each frame because everything moves
    QHash<qint64, QList<Enemy*>> grid;
    grid.reserve(enemyList.size() * 4);

    for(Enemy* enemy : std::as_const(enemyList))
    {
        const int minCX = cellIndex(enemy->x());
        const int maxCX = cellIndex(enemy->x() + enemySize);
        const int minCY = cellIndex(enemy->y());
        const int maxCY = cellIndex(enemy->y() + enemySize);

        for(int cx = minCX; cx <= maxCX; cx++){
            for(int cy = minCY; cy <= maxCY; cy++){
                grid[cellKey(cx, cy)].append(enemy);
            }
        }
    }

    //an enemy sits in up to 4 cells, so the same pointer can be visited twice,
    //and it is already deleted after the first hit
    QSet<Enemy*> destroyed;

    for(int i = bulletList.size() -1; i>=0; i--)
    {
        Bullet* bullet = bulletList[i];

        const double bulletLeft = bullet->x();
        const double bulletRight = bulletLeft + bulletWidth;
        const double bulletTop = bullet->y();
        const double bulletBottom = bulletTop + bulletHeight;

        const int minCX = cellIndex(bulletLeft);
        const int maxCX = cellIndex(bulletRight);
        const int minCY = cellIndex(bulletTop);
        const int maxCY = cellIndex(bulletBottom);

        Enemy* hit = nullptr;

        for(int cx = minCX; cx <= maxCX && !hit; cx++){
            for(int cy = minCY; cy <= maxCY && !hit; cy++){

                const auto cell = grid.constFind(cellKey(cx, cy));
                if(cell == grid.constEnd()){
                    continue;
                }

                for(Enemy* enemy : *cell)
                {
                    if(destroyed.contains(enemy)){
                        continue;
                    }

                    const double enemyLeft = enemy->x();
                    const double enemyRight = enemyLeft + enemySize;
                    const double enemyTop = enemy->y();
                    const double enemyBottom = enemyTop + enemySize;

                    if(bulletRight > enemyLeft && bulletLeft < enemyRight
                        && bulletTop < enemyBottom && bulletBottom > enemyTop)
                    {
                        hit = enemy;
                        break;
                    }
                }
            }
        }

        if(hit)
        {
            destroyed.insert(hit);
            deleteBullet(bullet);
            deleteEnemy(hit);
        }
    }
}

void Controller::updateMovement()
{
    if(moveDirection == -1)
    {
        setX(qMax(minX, m_x - xSpeed));
    }
    else if(moveDirection == 1)
    {
        setX(qMin(maxX, m_x + xSpeed));
    }
}