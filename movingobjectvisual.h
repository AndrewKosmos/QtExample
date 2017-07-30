#ifndef MOVINGOBJECTVISUAL_H
#define MOVINGOBJECTVISUAL_H

#include <QGraphicsItem>
#include <QPainter>

class MovingObjectVisual : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    MovingObjectVisual();
    ~MovingObjectVisual();

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
signals:
    
public slots:
    
};

#endif // MOVINGOBJECTVISUAL_H
