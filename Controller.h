#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QTimer>
class Controller: public QObject
{
    Q_OBJECT
    Q_PROPERTY(double x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(double y READ y WRITE setY NOTIFY yChanged)

public:
    Controller(QObject* parent = nullptr);
    Q_INVOKABLE void setBoundaries(double width, double height);
    double x(){
        return m_x;
    }
    double y(){
        return m_y;
    }

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
    Q_INVOKABLE void moveLeft(){
        setX(qMax(minX, m_x - xSpeed));
    }

    Q_INVOKABLE void moveRight(){
        setX(qMin(maxX, m_x + xSpeed));
    }
    Q_INVOKABLE void applyThrust(){
        ySpeed= maxThrust;
        if(m_y < bottomY/1.5){
            ySpeed = 0;
        }
    }
public slots:
    void updateState(){
        m_y += ySpeed;
        ySpeed += gravity;

        if(m_y > bottomY){
            m_y = bottomY;
        }
        emit yChanged();
    }
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
