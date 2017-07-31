#ifndef VISUALINFO_H
#define VISUALINFO_H

#include <QGraphicsItem>
#include <QString>
#include <QRect>
#include <QPainter>

class visualInfo : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    visualInfo(QString x,QString y,QString time,QString distance);
    QString currentX;
    QString currentY;
    QString time;
    QString distance;
protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
signals:
    
public slots:
    
};

#endif // VISUALINFO_H
