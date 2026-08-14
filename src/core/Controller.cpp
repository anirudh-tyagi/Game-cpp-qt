#include "core/Controller.h"

#include "core/GameConfig.h"

#include <QHash>
#include <QRandomGenerator>
#include <QSet>
#include <cmath>
#include <utility>

namespace {
using namespace GameConfig;

int cellIndex(double value){
    return static_cast<int>(std::floor(value / cellSize));
}

//pack the two cell coords into one key so we can use a flat QHash
qint64 cellKey(int cx, int cy){
    return (static_cast<qint64>(cx) << 32) | static_cast<quint32>(cy);
}

int nextSpawnDelay(){
    return enemySpawnMinMs + QRandomGenerator::global()->bounded(enemySpawnJitterMs);
}
}

Controller::Controller(QObject* parent)
    : QObject(parent),
    m_x(50),
    m_y(50),
    xSpeed(GameConfig::playerXSpeed),
    ySpeed(10),
    minX(0),
    maxX(0),
    bottomY(0),
    moveDirection(0)
{
    connect(&time, &QTimer::timeout, this, &Controller::updateState);
    time.start(GameConfig::frameIntervalMs); //60fps

    connect(&move, &QTimer::timeout, this, &Controller::updateMovement);
    move.setInterval(GameConfig::frameIntervalMs); //60fps, started only while a key is held

    connect(&startE, &QTimer::timeout, this, &Controller::createEnemies);
    startE.start(nextSpawnDelay()); //1-3 seconds
}

double Controller::despawnY() const
{
    //bottomY is height - playerHeight, so this is one enemy height below the window
    return bottomY + (2 * GameConfig::enemyHeight);
}

void Controller::setBoundaries(double width, double height)
{
    minX = 0;
    maxX = width - GameConfig::playerWidth;

    bottomY = height - GameConfig::playerHeight;

    // Start player at bottom
    m_y = bottomY;

    // enemies spawned before a resize would otherwise keep the old cutoff
    for(Enemy* enemy : std::as_const(enemyList)){
        enemy->setDespawnY(despawnY());
    }

    emit yChanged();
}

bool Controller::enemyReachedBottom()
{
    //an enemy box runs from y to y + enemyHeight, and the window bottom sits at
    //bottomY + playerHeight, so touching down means y has caught up with bottomY
    for(Enemy* enemy : std::as_const(enemyList)){
        if(enemy->y() >= bottomY){
            return true;
        }
    }
    return false;
}

bool Controller::enemyHitPlayer()
{
    //only one player box, so a plain sweep beats building a grid for it
    const double playerLeft = m_x;
    const double playerRight = playerLeft + GameConfig::playerWidth;
    const double playerTop = m_y;
    const double playerBottom = playerTop + GameConfig::playerHeight;

    for(Enemy* enemy : std::as_const(enemyList))
    {
        const double enemyLeft = enemy->x();
        const double enemyRight = enemyLeft + GameConfig::enemyWidth;
        const double enemyTop = enemy->y();
        const double enemyBottom = enemyTop + GameConfig::enemyHeight;

        if(playerRight > enemyLeft && playerLeft < enemyRight
            && playerTop < enemyBottom && playerBottom > enemyTop)
        {
            return true;
        }
    }
    return false;
}

void Controller::endGame()
{
    if(m_gameOver){
        return;
    }

    m_gameOver = true;

    //every entity drives itself off its own timer, so freeze them all and the
    //last frame stays on screen instead of the world carrying on underneath
    time.stop();
    move.stop();
    startE.stop();
    moveDirection = 0;

    for(Enemy* enemy : std::as_const(enemyList)){
        enemy->freeze();
    }
    for(Bullet* bullet : std::as_const(bulletList)){
        bullet->freeze();
    }

    emit gameOverChanged();
}

void Controller::moveLeft()
{
    if(m_gameOver){
        return;
    }

    moveDirection = -1;
    updateMovement(); //respond to the very first press without waiting a tick
    if (!move.isActive())
    {
        move.start();
    }
}

void Controller::moveRight(){
    if(m_gameOver){
        return;
    }

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
    if(m_gameOver){
        return;
    }

    ySpeed = GameConfig::maxThrust;
    if(m_y < bottomY/1.5){
        ySpeed = 0;
    }
}

void Controller::fireBullet()
{
    if(m_gameOver){
        return;
    }

    Bullet* newBullet = new Bullet(this, this);
    //centre the bullet on the player
    newBullet->setX(m_x + (GameConfig::playerWidth - GameConfig::bulletWidth) / 2);
    newBullet->setY(m_y);
    bulletList.append(newBullet);
    emit bulletChanged();
}

void Controller::createEnemies()
{
    if(m_gameOver){
        return;
    }

    startE.start(nextSpawnDelay());

    //nothing to spawn into until QML has handed us the window size
    if(maxX <= 0){
        return;
    }

    Enemy* newEnemy = new Enemy(this, this);
    newEnemy->setX(QRandomGenerator::global()->bounded(static_cast<int>(maxX) + 1));
    newEnemy->setY(-GameConfig::enemyHeight);
    newEnemy->setDespawnY(despawnY());
    enemyList.append(newEnemy);

    emit enemyChanged();
}

//slot
void Controller::updateState(){
    m_y += ySpeed;
    ySpeed += GameConfig::gravity;

    if(m_y > bottomY){
        m_y = bottomY;
    }
    checkCollision();
    emit yChanged();

    //checked after the collision pass so an enemy shot on its last frame
    //still counts as killed rather than as a landing or a crash
    if(enemyReachedBottom() || enemyHitPlayer()){
        endGame();
    }
}

void Controller::deleteBullet(Bullet *bullet)
{
    int index= bulletList.indexOf(bullet);
    if (index != -1)
    {
        //out of the list first, so QML never re-reads the model while a
        //pointer to a dying bullet is still in it
        bulletList.removeAt(index);
        emit bulletChanged();

        //this normally runs from the bullet's own timeout, so a plain delete
        //would free the object underneath its own call stack, stop its timer
        //so it cannot ask to be deleted again while the event loop catches up
        bullet->freeze();
        bullet->deleteLater();
    }
}


void Controller::deleteEnemy(Enemy *enemy)
{
    int index= enemyList.indexOf(enemy);
    if (index != -1)
    {
        //same ordering as deleteBullet, list first then the object
        enemyList.removeAt(index);
        emit enemyChanged();

        enemy->freeze();
        enemy->deleteLater();
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
        const int maxCX = cellIndex(enemy->x() + GameConfig::enemyWidth);
        const int minCY = cellIndex(enemy->y());
        const int maxCY = cellIndex(enemy->y() + GameConfig::enemyHeight);

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
        const double bulletRight = bulletLeft + GameConfig::bulletWidth;
        const double bulletTop = bullet->y();
        const double bulletBottom = bulletTop + GameConfig::bulletHeight;

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
                    const double enemyRight = enemyLeft + GameConfig::enemyWidth;
                    const double enemyTop = enemy->y();
                    const double enemyBottom = enemyTop + GameConfig::enemyHeight;

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
            //setScore already emits scoreChanged
            setScore(score() + GameConfig::pointsPerKill);
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
