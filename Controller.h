#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QQmlListProperty>

#include <Bullet.h>
#include <Enemy.h>

#include <vector>
class Controller: public QObject
{
    Q_OBJECT
    Q_PROPERTY(double x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(double y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(QQmlListProperty<Bullet> bullets READ bullets NOTIFY bulletChanged)
    Q_PROPERTY(QQmlListProperty<Enemy> enemies READ enemies NOTIFY enemyChanged)

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
    Q_INVOKABLE void createEnemies();
    //this helps us exopse our C++ obj or class to qmls
    QQmlListProperty <Bullet> bullets()
    {
        return QQmlListProperty(this, &bulletList);
    }
    QQmlListProperty <Enemy> enemies()
    {
        return QQmlListProperty(this, &enemyList);
    }
    Q_INVOKABLE void stopMovement();
public slots:
    void updateState();
    void deleteBullet(Bullet* bullet);
    void deleteEnemy(Enemy *enemy);
    void checkCollision();
    void updateMovement();
signals:
    void xChanged();
    void yChanged();
    void bulletChanged();
    void enemyChanged();
    void bulletDestroyed();

private:
    double despawnY() const; //y below which an enemy is fully off screen
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
    QTimer startE;
    QList<Bullet*> bulletList;
    QList<Enemy*> enemyList;
    int moveDirection;
    QTimer move;
};

#endif // CONTROLLER_H
