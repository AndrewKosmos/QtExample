#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGlobal>
#include <QDebug>
#include <qmath.h>
#include <qnumeric.h>
#include <QGraphicsScene>
#include <movingobject.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
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
};

#endif // MAINWINDOW_H