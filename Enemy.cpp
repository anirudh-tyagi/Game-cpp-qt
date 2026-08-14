#include "Enemy.h"
#include <Controller.h>

Enemy::Enemy(Controller* controller, QObject* parent):
    QObject(parent),
    m_x(0),
    m_y(-50),
    ySpeed(1),
    m_despawnY(0)
{
    connect(&eTime, &QTimer::timeout, this, &Enemy::updateEnemy);
    eTime.start(50);

    connect(this, &Enemy::enemyDestroyed, controller, &Controller::deleteEnemy);
}

void Enemy::updateEnemy()
{
    setY(m_y + ySpeed);
    if(m_y > m_despawnY){
        emit enemyDestroyed(this);
    }
}

