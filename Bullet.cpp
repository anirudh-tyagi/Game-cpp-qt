#include "Bullet.h"

Bullet::Bullet(QObject* parent):
    m_x(),
    m_y(),
    ySpeed(-10)
{
    connect(&bTime, &QTimer::timeout, this, &Bullet::updateBullet);
    bTime.start(16); //60fps
}

void Bullet::updateBullet()
{
    setY(m_y + ySpeed);
}

