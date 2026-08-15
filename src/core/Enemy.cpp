#include "core/Enemy.h"

#include "core/Controller.h"
#include "core/GameConfig.h"

#include <QRandomGenerator>

Enemy::Enemy(Controller* controller, QObject* parent):
    QObject(parent),
    m_x(0),
    m_y(-GameConfig::enemyHeight),
    ySpeed(GameConfig::enemyYSpeed),
    m_despawnY(0),
    m_sprite(1 + QRandomGenerator::global()->bounded(spriteCount))
{
    connect(&eTime, &QTimer::timeout, this, &Enemy::updateEnemy);
    eTime.start(GameConfig::enemyFrameIntervalMs);

    connect(this, &Enemy::enemyDestroyed, controller, &Controller::deleteEnemy);
}

void Enemy::updateEnemy()
{
    setY(m_y + ySpeed);
    if(m_y > m_despawnY){
        emit enemyDestroyed(this);
    }
}
