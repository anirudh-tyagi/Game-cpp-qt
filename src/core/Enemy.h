#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QTimer>

#include "core/GameConfig.h"

//A falling enemy. Unlike Bullet it has no self-destruct path: a run ends the
//moment an enemy passes the floor line, so one can never outlive the run it
//belongs to. The controller drops them all through clearEntities() on the next
//start, and shooting one removes it directly.
class Enemy: public QObject
{
    Q_OBJECT
    Q_PROPERTY(double x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(double y READ y WRITE setY NOTIFY yChanged)
    //which of the sprites this one wears, 1..spriteCount. Picked here rather
    //than in the delegate so it survives the view rebuilding its items
    Q_PROPERTY(int sprite READ sprite CONSTANT)
public:
    //number of enemy*.png variants shipped in assets/
    static constexpr int spriteCount = 5;

    explicit Enemy(QObject* parent = nullptr);
    double x(){return m_x;}
    double y(){return m_y;}
    int sprite() const {return m_sprite;}

    //set from the controller at spawn, so a wave keeps the speed of the level
    //it arrived in even if the player levels up mid-descent
    void setYSpeed(double value){ySpeed = value;}

    //stop moving but stay on screen, used when the game ends or is paused
    void freeze(){eTime.stop();}
    //pick the descent back up after a pause
    void unfreeze(){eTime.start(GameConfig::enemyFrameIntervalMs);}

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
private:
    double m_x;
    double m_y;
    double ySpeed;
    int m_sprite;
    QTimer eTime;

};

#endif // ENEMY_H
