#ifndef CLASS2_H
#define CLASS2_H

#include <QObject>
#include <QDebug>
class Class2: public QObject
{
    Q_OBJECT
public:
    Class2();
public:
    void print(){
        qDebug()<<"I have printed";
    }
};

#endif // CLASS2_H
