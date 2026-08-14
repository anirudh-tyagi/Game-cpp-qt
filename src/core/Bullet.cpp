#include "core/Bullet.h"

#include "core/Controller.h"
#include "core/GameConfig.h"

Bullet::Bullet(Controller* controller, QObject* parent)
    : QObject(parent),
    m_x(),
    m_y(),
    ySpeed(GameConfig::bulletYSpeed)
{
    connect(&bTime, &QTimer::timeout, this, &Bullet::updateBullet);
    bTime.start(GameConfig::frameIntervalMs);

    connect(this, &Bullet::bulletDestroyed, controller, &Controller::deleteBullet);
}

void Bullet::updateBullet()
{
    setY(m_y + ySpeed);
    if(m_y < 0){
        emit bulletDestroyed(this);
    }
}
