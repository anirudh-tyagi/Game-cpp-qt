#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QQmlListProperty>
#include <QTimer>

#include "core/Bullet.h"
#include "core/Enemy.h"

class Controller: public QObject
{
    Q_OBJECT
    Q_PROPERTY(double x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(double y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(QQmlListProperty<Bullet> bullets READ bullets NOTIFY bulletChanged)
    Q_PROPERTY(QQmlListProperty<Enemy> enemies READ enemies NOTIFY enemyChanged)
    Q_PROPERTY(double score READ score WRITE setScore NOTIFY scoreChanged)
    Q_PROPERTY(bool gameOver READ gameOver NOTIFY gameOverChanged)

public:
    Controller(QObject* parent = nullptr);

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
    void setScore(double value){if(m_score != value){m_score=value; emit scoreChanged();}}
    double score(){return m_score;}
    bool gameOver() const {return m_gameOver;}

    //this helps us expose our C++ obj or class to qmls
    QQmlListProperty <Bullet> bullets()
    {
        return QQmlListProperty(this, &bulletList);
    }
    QQmlListProperty <Enemy> enemies()
    {
        return QQmlListProperty(this, &enemyList);
    }

    //called from QML
    Q_INVOKABLE void setBoundaries(double width, double height);
    Q_INVOKABLE void moveLeft();
    Q_INVOKABLE void moveRight();
    Q_INVOKABLE void stopMovement();
    Q_INVOKABLE void applyThrust();
    Q_INVOKABLE void fireBullet();
    Q_INVOKABLE void createEnemies();

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
    void scoreChanged();
    void gameOverChanged();

private:
    double despawnY() const; //y below which an enemy is fully off screen
    bool enemyReachedBottom(); //true once any enemy touches the bottom edge
    bool enemyHitPlayer(); //true once any enemy box overlaps the player box
    void endGame(); //freeze everything and flag the game as over

    double m_x; //current position of rect on x direction
    double m_y; //current position of rect on y dirsction
    double xSpeed;
    double ySpeed;
    double minX;
    double maxX;
    double bottomY;
    QTimer time;
    QTimer startE;
    QList<Bullet*> bulletList;
    QList<Enemy*> enemyList;
    int moveDirection;
    QTimer move;
    double m_score = 0;
    bool m_gameOver = false;
};

#endif // CONTROLLER_H
