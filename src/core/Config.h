#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

#include "core/GameConfig.h"

//Read-only window onto the entity sizes in GameConfig.h, so the view can draw
//the same boxes the simulation collides with instead of restating the numbers.
//Exposed to QML as the "config" context property, alongside "control".
//
//Only the collision-relevant sizes live here. Colours, fonts and durations are
//presentation and stay in src/ui/Theme.qml.
class Config: public QObject
{
    Q_OBJECT
    Q_PROPERTY(double playerWidth READ playerWidth CONSTANT)
    Q_PROPERTY(double playerHeight READ playerHeight CONSTANT)
    Q_PROPERTY(double enemyWidth READ enemyWidth CONSTANT)
    Q_PROPERTY(double enemyHeight READ enemyHeight CONSTANT)
    Q_PROPERTY(double bulletWidth READ bulletWidth CONSTANT)
    Q_PROPERTY(double bulletHeight READ bulletHeight CONSTANT)

public:
    using QObject::QObject;

    double playerWidth() const {return GameConfig::playerWidth;}
    double playerHeight() const {return GameConfig::playerHeight;}
    double enemyWidth() const {return GameConfig::enemyWidth;}
    double enemyHeight() const {return GameConfig::enemyHeight;}
    double bulletWidth() const {return GameConfig::bulletWidth;}
    double bulletHeight() const {return GameConfig::bulletHeight;}
};

#endif // CONFIG_H
