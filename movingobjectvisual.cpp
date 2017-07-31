#include "movingobjectvisual.h"

MovingObjectVisual::MovingObjectVisual():QObject(),QGraphicsItem()
{
}

MovingObjectVisual::~MovingObjectVisual()
{
}

QRectF MovingObjectVisual::boundingRect() const
{
    return QRectF(-5,-4,10,8);
}

void MovingObjectVisual::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPolygon polygon;
    //polygon << QPoint(0,-4) << QPoint(5,4) << QPoint(-5,4);
    polygon << QPoint(5,0) << QPoint(-5,4) << QPoint(-5,-4);
    painter->setBrush(Qt::gray);
    painter->drawPolygon(polygon);
}
