#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QTimer>

class Controller;

class Enemy: public QObject
{
    Q_OBJECT
    Q_PROPERTY(double x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(double y READ y WRITE setY NOTIFY yChanged)
public:


    Enemy(Controller* controller, QObject* parent = nullptr);
    double x(){return m_x;}
    double y(){return m_y;}

    //y past which the enemy has left the screen and cleans itself up
    void setDespawnY(double value){m_despawnY = value;}

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
public slots:
    void updateEnemy();
signals:
    void xChanged();
    void yChanged();
    void enemyDestroyed(Enemy* enemy);
private:
    double m_x;
    double m_y;
    double ySpeed;
    double m_despawnY;
    QTimer eTime;

};

#endif // ENEMY_H
