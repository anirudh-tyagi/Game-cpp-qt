#include "Bullet.h"
#include <Controller.h>

Bullet::Bullet(Controller* controller, QObject* parent)
    : QObject(parent),
    m_x(),
    m_y(),
    ySpeed(-10)
{
    connect(&bTime, &QTimer::timeout, this, &Bullet::updateBullet);
    bTime.start(16); //60fps

    connect(this, &Bullet::bulletDestroyed, controller, &Controller::deleteBullet);
}

void Bullet::updateBullet()
{
    setY(m_y + ySpeed);
    if(m_y < 0){
        emit bulletDestroyed(this);
    }
}

