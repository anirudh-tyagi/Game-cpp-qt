#include "Controller.h"
#include <qdebug.h>

Controller::Controller(QObject* parent)
    : QObject(parent),
    m_x(50),
    m_y(50),
    xSpeed(10),
    ySpeed(10),
    minX(0),
    maxX(0),
    bottomY(0)
{
    connect(&time, &QTimer::timeout, this, &Controller::updateState);
    time.start(16); //60fps
}

void Controller::setBoundaries(double width, double height)
{
    minX = 0;
    maxX = width - 50;

    bottomY = height - 50;

    // Start player at bottom
    m_y = bottomY;

    emit yChanged();
}

void Controller::moveLeft()
{
    setX(qMax(minX, m_x - xSpeed));
}

void Controller::moveRight(){
    setX(qMin(maxX, m_x + xSpeed));
}

void Controller::applyThrust(){
    ySpeed= maxThrust;
    if(m_y < bottomY/1.5){
        ySpeed = 0;
    }
}

Q_INVOKABLE void Controller::fireBullet()
{
    Bullet* newBullet = new Bullet();
    newBullet->setX(m_x + 25);
    newBullet->setY(m_y);
    bulletList.append(newBullet);
    emit bulletChanged();
}

Q_INVOKABLE void Controller::createEnemies()
{
    Enemy* newEnemy = new Enemy();
    newEnemy->setX(rand() % 1512);
    newEnemy->setY(0);
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
    emit yChanged();
}