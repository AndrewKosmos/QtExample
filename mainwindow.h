#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGlobal>
#include <QDebug>
#include <qmath.h>
#include <qnumeric.h>
#include <QGraphicsScene>
#include <movingobject.h>
#include <QMessageBox>
#include <movingobjectvisual.h>
#include <visualinfo.h>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MovingObject *obj1;
    MovingObject *obj2;
    qreal zeroLine;
    QLineF *line1;
    QLineF *line2;
    QTimer *timer;
    int timePastStart;

    explicit MainWindow(QWidget *parent = 0);
    qreal findIntersectX(float initX1,float initX2,float initY1,float initY2,int AngleA, int AngleB);
    qreal findIntersectY(float initY,float intersectX,float initX,int Angle);
    qreal convertGradsToRadians(int Angle);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    qreal intersectX;
    qreal intersectY;
    static const double PI = 3.1415926535;

    QGraphicsScene *scene;

    void drawLines(MovingObject *obj1,MovingObject *obj2);
    void drawObjects(int Angle1,int Angle2,qreal X1,qreal Y1,qreal X2,qreal Y2);
    void drawInfoBoxes(qreal X1,qreal Y1, qreal X2,qreal Y2, qreal Distance1, qreal Distance2, qreal Time1, qreal Time2);
    void calculateIntersection(MovingObject *obj1,MovingObject *obj2);

    qreal calculateDistance(qreal currentX, qreal currentY,qreal intersectX,qreal intersectY);
    qreal calculateIntersectionTime(MovingObject *obj,qreal distance);
    QPointF calculateCurrentCoordinates(MovingObject *obj,qreal time);

public slots:
    void startCalculating();
    void UpdateTime();
};

#endif // MAINWINDOW_H
