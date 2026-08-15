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

//waves arrive closer together as the level climbs, down to a floor that keeps
//the screen survivable
int nextSpawnDelay(int level){
    const double scale = std::pow(spawnScalePerLevel, level - 1);

    const int minMs = qMax(enemySpawnFloorMs,
                           static_cast<int>(enemySpawnMinMs * scale));
    const int jitterMs = qMax(enemySpawnJitterFloorMs,
                              static_cast<int>(enemySpawnJitterMs * scale));

    return minMs + QRandomGenerator::global()->bounded(jitterMs);
}

//and they fall faster, up to a cap
double enemySpeedForLevel(int level){
    const double speed = enemyYSpeed + ((level - 1) * enemySpeedPerLevel);
    return qMin(speed, enemyMaxYSpeed);
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
    maxEnemyX(0),
    bottomY(0),
    moveDirection(0)
{
    //nothing runs until the start menu calls startGame(), the timers are only
    //wired up here
    connect(&time, &QTimer::timeout, this, &Controller::updateState);
    time.setInterval(GameConfig::frameIntervalMs); //60fps

    connect(&move, &QTimer::timeout, this, &Controller::updateMovement);
    move.setInterval(GameConfig::frameIntervalMs); //60fps, started only while a key is held

    connect(&startE, &QTimer::timeout, this, &Controller::createEnemies);
}

void Controller::clearEntities()
{
    //same ordering as deleteBullet/deleteEnemy, list first then the objects, so
    //QML cannot re-read the model while a dying pointer is still in it
    const QList<Bullet*> bullets = std::move(bulletList);
    bulletList.clear();
    if(!bullets.isEmpty()){
        emit bulletChanged();
    }

    const QList<Enemy*> enemies = std::move(enemyList);
    enemyList.clear();
    if(!enemies.isEmpty()){
        emit enemyChanged();
    }

    for(Bullet* bullet : bullets){
        bullet->freeze();
        bullet->deleteLater();
    }
    for(Enemy* enemy : enemies){
        enemy->freeze();
        enemy->deleteLater();
    }
}

void Controller::updateLevel()
{
    const int level = 1 + static_cast<int>(m_score / GameConfig::pointsPerLevel);

    if(m_level != level){
        m_level = level;
        emit levelChanged();
    }
}

void Controller::setThrusting(bool value)
{
    if(m_thrusting != value){
        m_thrusting = value;
        emit thrustingChanged();
    }
}

void Controller::startGame()
{
    clearEntities();

    //a restart has to undo everything endGame() froze. The score going back to
    //zero drops the level with it
    setScore(0);
    setThrusting(false);
    ySpeed = 0;
    moveDirection = 0;
    m_y = bottomY;
    emit yChanged();
    setX(maxX > 0 ? maxX / 2 : m_x); //start centred

    if(m_gameOver){
        m_gameOver = false;
        emit gameOverChanged();
    }

    if(m_paused){
        m_paused = false;
        emit pausedChanged();
    }

    m_running = true;
    emit runningChanged();

    time.start();
    startE.start(nextSpawnDelay(m_level)); //1-3 seconds at level 1
}

void Controller::togglePause()
{
    //only a live run can be paused. There is nothing to freeze on the start
    //page or the game-over page, and unpausing into them would be worse
    if(!m_running){
        return;
    }

    m_paused = !m_paused;

    if(m_paused){
        //hold the spawn timer where it stands, otherwise a pause would hand the
        //player a full fresh interval before the next wave
        pausedSpawnMs = qMax(1, startE.remainingTime());

        time.stop();
        move.stop();
        startE.stop();
        setThrusting(false);

        //every entity drives itself off its own timer, so the world carries on
        //underneath unless each one is stopped too
        for(Enemy* enemy : std::as_const(enemyList)){
            enemy->freeze();
        }
        for(Bullet* bullet : std::as_const(bulletList)){
            bullet->freeze();
        }
    }
    else {
        for(Enemy* enemy : std::as_const(enemyList)){
            enemy->unfreeze();
        }
        for(Bullet* bullet : std::as_const(bulletList)){
            bullet->unfreeze();
        }

        time.start();
        startE.start(pausedSpawnMs);
        //the move timer stays down, it only runs while an arrow is held and the
        //key release may well have happened while we were paused
        moveDirection = 0;
    }

    emit pausedChanged();
}

double Controller::despawnY() const
{
    //bottomY is height - playerHeight, so this is one enemy height below the window.
    //A live run ends the moment an enemy passes bottomY, well short of this, so
    //this is only a safety net for an enemy that somehow outlives that check
    return bottomY + (2 * GameConfig::enemyHeight);
}

double Controller::ceilingY() const
{
    return bottomY * GameConfig::ceilingFraction;
}

void Controller::setBoundaries(double width, double height)
{
    minX = 0;
    maxX = width - GameConfig::playerWidth;
    //enemies are sized off enemyWidth, not playerWidth, so they get their own
    //right edge. The two constants happen to match today
    maxEnemyX = width - GameConfig::enemyWidth;

    bottomY = height - GameConfig::playerHeight;

    if(m_running){
        //a resize mid-flight must not drop the ship out of the sky, only pull it
        //back inside a window that has shrunk under it
        if(m_y > bottomY){
            m_y = bottomY;
            emit yChanged();
        }
    }
    else {
        m_y = bottomY; //park it on the floor, ready for the next run
        emit yChanged();
    }

    //a narrower window can leave the ship outside the new right edge, where it
    //would sit until the player happened to press left
    setX(qBound(minX, m_x, qMax(minX, maxX)));

    // enemies spawned before a resize would otherwise keep the old cutoff
    for(Enemy* enemy : std::as_const(enemyList)){
        enemy->setDespawnY(despawnY());
    }
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
    m_running = false;

    //keeps "paused implies a run to go back to" true unconditionally
    if(m_paused){
        m_paused = false;
        emit pausedChanged();
    }

    //every entity drives itself off its own timer, so freeze them all and the
    //last frame stays on screen instead of the world carrying on underneath
    time.stop();
    move.stop();
    startE.stop();
    moveDirection = 0;
    setThrusting(false);

    for(Enemy* enemy : std::as_const(enemyList)){
        enemy->freeze();
    }
    for(Bullet* bullet : std::as_const(bulletList)){
        bullet->freeze();
    }

    emit gameOverChanged();
    emit runningChanged();
}

void Controller::moveLeft()
{
    if(!m_running || m_paused){
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
    if(!m_running || m_paused){
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
    if(!m_running || m_paused){
        return;
    }

    //no altitude test here. The ceiling is a property of the world, so
    //updateState() enforces it every frame, the same way it does the floor.
    //Testing it on the key press alone only gated the input, and thrust applied
    //just below the line still carried the ship clean through it
    ySpeed = GameConfig::maxThrust;
}

void Controller::fireBullet()
{
    if(!m_running || m_paused){
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
    if(!m_running || m_paused){
        return;
    }

    startE.start(nextSpawnDelay(m_level));

    //nothing to spawn into until QML has handed us the window size
    if(maxEnemyX <= 0){
        return;
    }

    Enemy* newEnemy = new Enemy(this, this);
    //sized off enemyWidth, so the whole sprite lands on screen and the right
    //edge stays reachable even if the enemy and player boxes ever differ
    newEnemy->setX(QRandomGenerator::global()->bounded(static_cast<int>(maxEnemyX) + 1));
    newEnemy->setY(-GameConfig::enemyHeight);
    newEnemy->setDespawnY(despawnY());
    newEnemy->setYSpeed(enemySpeedForLevel(m_level));
    enemyList.append(newEnemy);

    emit enemyChanged();
}

//slot
void Controller::updateState(){
    double newY = m_y + ySpeed;
    ySpeed += GameConfig::gravity;

    //the world is a corridor, and both ends stop the ship rather than only
    //catching its position. Letting ySpeed run on past a clamp left it growing
    //without bound while the ship sat still on the floor
    if(newY > bottomY){
        newY = bottomY;
        ySpeed = 0;
    }
    else if(newY < ceilingY()){
        newY = ceilingY();
        ySpeed = 0;
    }

    //the plume burns for as long as the ship is still gaining height, a flag on
    //the key press alone would blink out after a single frame
    setThrusting(ySpeed < 0);

    //setY already guards on inequality, so a ship resting on the floor stops
    //re-evaluating its binding sixty times a second
    setY(newY);

    checkCollision();

    //checked after the collision pass so an enemy shot on its last frame
    //still counts as killed rather than as a landing or a crash
    if(enemyReachedBottom() || enemyHitPlayer()){
        endGame();
    }
}

bool Controller::removeBullet(Bullet *bullet)
{
    int index= bulletList.indexOf(bullet);
    if (index == -1)
    {
        return false;
    }

    //out of the list first, so QML never re-reads the model while a
    //pointer to a dying bullet is still in it
    bulletList.removeAt(index);

    //this normally runs from the bullet's own timeout, so a plain delete
    //would free the object underneath its own call stack, stop its timer
    //so it cannot ask to be deleted again while the event loop catches up
    bullet->freeze();
    bullet->deleteLater();
    return true;
}

bool Controller::removeEnemy(Enemy *enemy)
{
    int index= enemyList.indexOf(enemy);
    if (index == -1)
    {
        return false;
    }

    //same ordering as removeBullet, list first then the object
    enemyList.removeAt(index);

    enemy->freeze();
    enemy->deleteLater();
    return true;
}

void Controller::deleteBullet(Bullet *bullet)
{
    if(removeBullet(bullet)){
        emit bulletChanged();
    }
}


void Controller::deleteEnemy(Enemy *enemy)
{
    if(removeEnemy(enemy)){
        emit enemyChanged();
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

    //a QQmlListProperty notify makes the Repeater tear down and rebuild every
    //delegate, so a frame with three kills used to cost six full rebuilds.
    //Collect the changes here and tell QML once, at the end
    bool bulletsChanged = false;
    bool enemiesChanged = false;

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
            bulletsChanged |= removeBullet(bullet);
            enemiesChanged |= removeEnemy(hit);
            //setScore already emits scoreChanged
            setScore(score() + GameConfig::pointsPerKill);
        }
    }

    if(bulletsChanged){
        emit bulletChanged();
    }
    if(enemiesChanged){
        emit enemyChanged();
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
