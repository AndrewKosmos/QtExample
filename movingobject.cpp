#include "movingobject.h"

MovingObject::MovingObject(float X, float Y, int Angle, int Velocity)
{
    this->initX = X;
    this->initY = Y;
    this->Angle = Angle;
    this->Velocity = Velocity;
}
