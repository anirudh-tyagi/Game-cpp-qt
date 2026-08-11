#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

class Controller
{
    Q_OBJECT
    Q_PROPERTY(type name READ name WRITE setName NOTIFY nameChanged FINAL)
private:
    double m_x; //current position of rect on x direction
    double m_y; //current position of rect on y dirsction
public:
    Controller();
};

#endif // CONTROLLER_H
