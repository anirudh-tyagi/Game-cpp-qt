#ifndef CLASS1_H
#define CLASS1_H
#include <QObject>
class Class1: public QObject
{
    Q_OBJECT
public:
    Class1();
signals:
    void printIt();
};



#endif // CLASS1_H
