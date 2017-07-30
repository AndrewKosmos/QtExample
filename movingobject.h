#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H

#include <QObject>

class MovingObject : public QObject
{
    Q_OBJECT

public:
    float initX;
    float initY;

    int Angle;
    int Velocity;
    MovingObject(float X, float Y, int Angle, int Velocity);
signals:
    
public slots:
    
};

#endif // MOVINGOBJECT_H
