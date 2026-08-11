#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

class Controller: public QObject
{
    Q_OBJECT
    Q_PROPERTY(double x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(double y READ y WRITE setY NOTIFY yChanged)

public:
    Controller(QObject* parent = nullptr);

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
        setX(m_x - xSpeed);
    }
    Q_INVOKABLE void moveRight(){
        setX(m_x + xSpeed);
    }
signals:
    void xChanged();
    void yChanged();

private:
    double m_x; //current position of rect on x direction
    double m_y; //current position of rect on y dirsction
    double xSpeed;

};

#endif // CONTROLLER_H
