#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QTimer>

#include <Bullet.h>
#include <vector>
class Controller: public QObject
{
    Q_OBJECT
    Q_PROPERTY(double x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(double y READ y WRITE setY NOTIFY yChanged)

public:
    Controller(QObject* parent = nullptr);
    Q_INVOKABLE void setBoundaries(double width, double height);
    double x(){return m_x;}
    double y(){return m_y;}

    void setX(double value){
        if(m_x != value){
            m_x = value;
            emit xChanged();
        }
    }
    void setY(double value){
        if(m_y != value){
            m_y = value;
            emit yChanged();
        }
    }
    Q_INVOKABLE void moveLeft();
    Q_INVOKABLE void moveRight();
    Q_INVOKABLE void applyThrust();

    Q_INVOKABLE void fireBullet();
public slots:
    void updateState();
signals:
    void xChanged();
    void yChanged();

private:
    double m_x; //current position of rect on x direction
    double m_y; //current position of rect on y dirsction
    double xSpeed;
    double ySpeed;
    double minX;
    double maxX;
    double bottomY;
    double maxThrust = -15;
    double gravity = 0.5;
    QTimer time;
};

#endif // CONTROLLER_H
