#include "core/Enemy.h"

#include "core/GameConfig.h"

#include <QRandomGenerator>

Enemy::Enemy(QObject* parent):
    QObject(parent),
    m_x(0),
    m_y(-GameConfig::enemyHeight),
    ySpeed(GameConfig::enemyYSpeed),
    m_sprite(1 + QRandomGenerator::global()->bounded(spriteCount))
{
    connect(&eTime, &QTimer::timeout, this, &Enemy::updateEnemy);
    eTime.start(GameConfig::enemyFrameIntervalMs);
}

void Enemy::updateEnemy()
{
    setY(m_y + ySpeed);
}
