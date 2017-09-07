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
#include <QTime>
#include <QFont>
#include <qtconcurrentrun.h>
#include <databaseworker.h>

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
    QTime *elapsedTime;
    int timePastStart;
    bool isPaused;

    explicit MainWindow(QWidget *parent = 0);
    qreal findIntersectX(float initX1,float initX2,float initY1,float initY2,int AngleA, int AngleB);
    qreal findIntersectY(float initY,float intersectX,float initX,int Angle);
    qreal convertGradsToRadians(int Angle);
    void pauseResumeMethod(bool value);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    qreal intersectX;
    qreal intersectY;
    qreal initX1,initX2,firstX1,firstX2;
    qreal initY1,initY2,firstY1,firstY2;
    int firstAngle1,firstVelocity1,firstAngle2,firstVelocity2;
    bool firstCalculating;
    qreal dX1,dX2,dY1,dY2;
    qreal newX1,newX2,newY1,newY2;
    QStringList values_list;
    QStringList objects_list;
    static const double PI = 3.1415926535;

    QGraphicsScene *scene;

    void drawLines(MovingObject *obj1,MovingObject *obj2);
    void drawObjects(int Angle1,int Angle2,qreal X1,qreal Y1,qreal X2,qreal Y2);
    void drawInfoBoxes(qreal X1,qreal Y1, qreal X2,qreal Y2, qreal Distance1, qreal Distance2, qreal Time1, qreal Time2);
    void drawInfoBoxes(qreal X1,qreal Y1, qreal X2,qreal Y2, QString Distance1, QString Distance2, QString Time1, QString Time2);
    void calculateIntersection(MovingObject *obj1,MovingObject *obj2);
    void updateUI();
    void setLineEditsState(bool state);

    qreal calculateDistance(qreal currentX, qreal currentY,qreal intersectX,qreal intersectY);
    qreal calculateIntersectionTime(MovingObject *obj,qreal distance);
    QPointF calculateCurrentCoordinates(MovingObject *obj,qreal time);

public slots:
    void startCalculating();
    void UpdateTime();
    void firstXchangedInPause();
    void firstYchangedInPause();
    void secondXchangedInPause();
    void secondYchangedInPause();
    void firstSpeedChanged();
    void secondSpeedChanged();
    void firstAngleChanged();
    void secondAngleChanged();
private slots:
    void on_pausePB_clicked();
    void on_stopPB_clicked();
    void on_lastCalcPB_clicked();

signals:
    void InsertValuesSignal(QStringList list);
    void UpdateValuesSignal(QString val, int pos);
    void InsertObjectsSignal(QStringList list,QStringList values);
    void UpdateLastVal();
    QStringList getLastData();
    bool haveLastData();
};

#endif // MAINWINDOW_H
