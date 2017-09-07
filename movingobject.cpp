#include "movingobject.h"

MovingObject::MovingObject(qreal X, qreal Y, int Angle, int Velocity)
{
    this->initX = X;
    this->initY = Y;
    this->Angle = Angle;
    this->Velocity = Velocity;
    currX = this->initX;
    currY = this->initY;
}

MovingObject::MovingObject(){}
