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

    zeroLine = scene->height();

    intersectX = 0;
    intersectY = 0;

    QRegExpValidator *numericValidator = new QRegExpValidator(QRegExp("\\d+"),this);
    ui->firstXLE->setValidator(numericValidator);
    ui->firstYLE->setValidator(numericValidator);
    ui->firstVelocityLE->setValidator(numericValidator);
    ui->firstAngleLE->setValidator(numericValidator);

    ui->secondXLE->setValidator(numericValidator);
    ui->secondYLE->setValidator(numericValidator);
    ui->secondVelocityLE->setValidator(numericValidator);
    ui->secondAngleLE->setValidator(numericValidator);

    connect(ui->startPB,SIGNAL(clicked()),this,SLOT(startCalculating()));
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

void MainWindow::drawLines(MovingObject *ob1,MovingObject *ob2)
{
    line1 = new QLineF(ob1->initX,zeroLine - ob1->initY,ob1->initX + this->width(),ob1->initY);
    line1->setAngle(ob1->Angle);
    scene->addLine(*line1,QPen(Qt::red));
    line2 = new QLineF(ob2->initX,zeroLine - ob2->initY,ob2->initX + this->width(),ob2->initY);
    line2->setAngle(ob2->Angle);
    scene->addLine(*line2,QPen(Qt::blue));
}

void MainWindow::drawObjects(int Angle1, int Angle2, qreal X1, qreal Y1, qreal X2, qreal Y2)
{
    MovingObjectVisual *obj1Visual = new MovingObjectVisual();
    obj1Visual->setRotation(-Angle1);
    scene->addItem(obj1Visual);
    obj1Visual->setPos(X1,zeroLine - Y1);

    MovingObjectVisual *obj2Visual = new MovingObjectVisual();
    obj2Visual->setRotation(-Angle2);
    scene->addItem(obj2Visual);
    obj2Visual->setPos(X2,zeroLine - Y2);
}

QPointF MainWindow::calculateCurrentCoordinates(MovingObject *obj,qreal time)
{
    qreal currX = obj->currX + obj->Velocity * time * qCos(convertGradsToRadians(obj->Angle));
    qreal currY = obj->currY + obj->Velocity * time * qSin(convertGradsToRadians(obj->Angle));
    return QPointF(currX,currY);
}

void MainWindow::drawInfoBoxes(qreal X1, qreal Y1, qreal X2, qreal Y2, qreal Distance1, qreal Distance2, qreal Time1, qreal Time2)
{
    visualInfo *info1 = new visualInfo(QString::number(X1),QString::number(Y1),QString::number(Time1),QString::number(Distance1));
    info1->setPos(X1 + 50,zeroLine - Y1 + 40);
    scene->addItem(info1);

    visualInfo *info2 = new visualInfo(QString::number(X2),QString::number(Y2),QString::number(Time2),QString::number(Distance2));
    info2->setPos(X2 + 50,zeroLine - Y2 + 40);
    scene->addItem(info2);
}

void MainWindow::calculateIntersection(MovingObject *obj1, MovingObject *obj2)
{
    qreal x = findIntersectX(obj1->initX,obj2->initX,obj1->initY,obj2->initY,obj1->Angle,obj2->Angle);
    qreal y = findIntersectY(obj1->initY,x,obj1->initX,obj1->Angle);
    if(qIsFinite(x) && !qIsNaN(x))
    {
        intersectX = x;
        intersectY = y;
    }
}

qreal MainWindow::calculateDistance(qreal currentX, qreal currentY, qreal intersectX, qreal intersectY)
{
    return qSqrt(qPow((intersectX - currentX),2) + qPow((intersectY - currentY),2));
}

qreal MainWindow::calculateIntersectionTime(MovingObject *obj, qreal distance)
{
    return distance / obj->Velocity;
}

void MainWindow::startCalculating()
{
    scene->clear();
    intersectX = intersectY = 0;
    timePastStart = 0;
    if(ui->firstXLE->text()!= "" && ui->firstYLE->text() != "" && ui->firstVelocityLE->text() != "" &&
            ui->firstAngleLE->text() != "" && ui->secondXLE->text() != "" && ui->secondYLE->text() != "" &&
            ui->secondVelocityLE->text() != "" && ui->secondAngleLE->text() != "")
    {
        qreal firstX = ui->firstXLE->text().toDouble();
        qreal firstY = ui->firstYLE->text().toDouble();
        int firstVelocity = ui->firstVelocityLE->text().toInt();
        int firstAngle = ui->firstAngleLE->text().toInt();

        qreal secondX = ui->secondXLE->text().toDouble();
        qreal secondY = ui->secondYLE->text().toDouble();
        int secondVelocity = ui->secondVelocityLE->text().toInt();
        int secondAngle = ui->secondAngleLE->text().toInt();

        obj1 = new MovingObject(firstX,firstY,firstAngle,firstVelocity);
        obj2 = new MovingObject(secondX,secondY,secondAngle,secondVelocity);

        drawLines(obj1,obj2);
        drawObjects(obj1->Angle,obj2->Angle,obj1->initX,obj1->initY,obj2->initX,obj2->initY);
        calculateIntersection(obj1,obj2);

        QPointF* intersectionPoint = new QPointF(intersectX,intersectY);

        if(intersectX && intersectY && (line1->intersect(*line2,intersectionPoint) == 1))
        {
            qDebug() << "X:" << intersectX;
            qDebug() << "Y:" << intersectY;

            ui->intersectionXLE->setText(QString::number(intersectX));
            ui->intersectionYLE->setText(QString::number(intersectY));

            scene->addEllipse(intersectX - 2.5,zeroLine - intersectY - 2.5,5,5,QPen(Qt::black),QBrush(Qt::black));

            qreal DistanceObj1 = calculateDistance(obj1->initX,obj1->initY,intersectX,intersectY);
            qreal IntersectTimeObj1 = calculateIntersectionTime(obj1,DistanceObj1);

            qDebug() << DistanceObj1 << "Time: " << IntersectTimeObj1;

            qreal DistanceObj2 = calculateDistance(obj2->initX,obj2->initY,intersectX,intersectY);
            qreal IntersectTimeObj2 = calculateIntersectionTime(obj2,DistanceObj2);

            qDebug() << DistanceObj2 << "Time: " << IntersectTimeObj2;

            drawInfoBoxes(obj1->initX,obj1->initY,obj2->initX,obj2->initY,DistanceObj1,DistanceObj2,IntersectTimeObj1,IntersectTimeObj2);

            timer = new QTimer();
            connect(timer,SIGNAL(timeout()),this,SLOT(UpdateTime()));
            timer->start(1000);
        }
        else
        {
            qDebug() << "NO INTERSEECTION!";
            ui->intersectionXLE->setText("-");
            ui->intersectionYLE->setText("-");
        }
    }
    else
    {
        QMessageBox::information(0, "Information", "Enter all required data!");
    }
}

void MainWindow::UpdateTime()
{
    scene->clear();
    drawLines(obj1,obj2);
    QPointF coordsObj1 = calculateCurrentCoordinates(obj1,timePastStart);
    QPointF coordsObj2 = calculateCurrentCoordinates(obj2,timePastStart);

    qreal DistanceObj1 = calculateDistance(coordsObj1.x(),coordsObj1.y(),intersectX,intersectY);
    qreal IntersectTimeObj1 = calculateIntersectionTime(obj1,DistanceObj1);


    qreal DistanceObj2 = calculateDistance(coordsObj2.x(),coordsObj2.y(),intersectX,intersectY);
    qreal IntersectTimeObj2 = calculateIntersectionTime(obj2,DistanceObj2);

    drawObjects(obj1->Angle,obj2->Angle,coordsObj1.x(),coordsObj1.y(),coordsObj2.x(),coordsObj2.y());
    drawInfoBoxes(coordsObj1.x(),coordsObj1.y(),coordsObj2.x(),coordsObj2.y(),DistanceObj1,DistanceObj2,IntersectTimeObj1,IntersectTimeObj2);

    qDebug() << "CURR_X1:" << coordsObj1.x() << "CURR_Y1" << coordsObj1.y() << "CURR_X2:" << coordsObj2.x() << "CURR_Y2" << coordsObj2.y();
    timePastStart++;
}
