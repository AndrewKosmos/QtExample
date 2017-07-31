#include "visualinfo.h"

visualInfo::visualInfo(QString x, QString y, QString time, QString distance) : QObject(), QGraphicsItem()
{
    currentX = x;
    currentY = y;
    this->time = time;
    this->distance = distance;
}

QRectF visualInfo::boundingRect() const
{
    return QRectF(-50,-40,100,80);
}

void visualInfo::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRect rectangle(-50,-40,100,80);
    painter->setPen(QPen(Qt::black));
    painter->drawRect(rectangle);

    painter->drawText(-50,-40,100,20,Qt::AlignCenter,QString("X:" + currentX));
    painter->drawText(-50,-20,100,20,Qt::AlignCenter,QString("Y:" + currentY));
    painter->drawText(-50,0,100,20,Qt::AlignCenter,QString("Intersect time:" + time));
    painter->drawText(-50,20,100,20,Qt::AlignCenter,QString("Distance:" + distance));
    Q_UNUSED(option);
    Q_UNUSED(widget);
}
