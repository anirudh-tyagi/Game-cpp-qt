#include "Controller.h"

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