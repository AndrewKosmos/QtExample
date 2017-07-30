#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    //ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //scene->setSceneRect(0,-500,500,500);
    qreal zeroLine = scene->height();

    intersectX = 0;
    intersectY = 0;
    MovingObject *ob1 = new MovingObject(0,0,45,20);
    MovingObject *ob2 = new MovingObject(200,400,315,10);

    qreal x = findIntersectX(ob1->initX,ob2->initX,ob1->initY,ob2->initY,ob1->Angle,ob2->Angle);
    qreal y = findIntersectY(ob1->initY,x,ob1->initX,ob1->Angle);
    if(qIsFinite(x) && !qIsNaN(x))
    {
        intersectX = x;
        intersectY = y;
    }

    if(intersectX && intersectY)
    {
        qDebug() << "X:" << intersectX;
        qDebug() << "Y:" << intersectY;
    }
    else
    {
        qDebug() << "NO INTERSEECTION!";
    }

    QLineF *line1 = new QLineF(ob1->initX,zeroLine - ob1->initY,ob1->initX + this->width(),ob1->initY);
    line1->setAngle(ob1->Angle);
    scene->addLine(*line1,QPen(Qt::red));
    QLineF *line2 = new QLineF(ob2->initX,zeroLine - ob2->initY,ob2->initX + this->width(),ob2->initY);
    line2->setAngle(ob2->Angle);
    scene->addLine(*line2,QPen(Qt::blue));

    scene->addEllipse(0,0,10,10,QPen(Qt::black),QBrush(Qt::black));
    scene->addEllipse(4,8,10,10,QPen(Qt::black),QBrush(Qt::black));
}


qreal MainWindow::findIntersectX(float initX1, float initX2, float initY1, float initY2, int AngleA, int AngleB)
{
   return (initY2 - initY1 + initX1*qTan(convertGradsToRadians(AngleA)) - initX2 * qTan(convertGradsToRadians(AngleB)))
          /(qTan(convertGradsToRadians(AngleA)) - qTan(convertGradsToRadians(AngleB)));
}

qreal MainWindow::findIntersectY(float initY, float intersectX, float initX, int Angle)
{
    return initY + qTan(convertGradsToRadians(Angle)) * (intersectX - initX);
}

qreal MainWindow::convertGradsToRadians(int Angle)
{
    return Angle * (PI/180);
}

MainWindow::~MainWindow()
{
    delete ui;
}
