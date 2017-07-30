#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H

#include <QObject>

class MovingObject : public QObject
{
    Q_OBJECT

public:
    qreal initX;
    qreal initY;

    int Angle;
    int Velocity;
    MovingObject(qreal X, qreal Y, int Angle, int Velocity);
    MovingObject();
signals:
    
public slots:
    
};

#endif // MOVINGOBJECT_H
