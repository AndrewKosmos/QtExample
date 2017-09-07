#include <QTextCodec>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //create and config graphics scene
    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    //remember initial OX axis
    zeroLine = scene->height();

    intersectX = 0;
    intersectY = 0;
    isPaused = false;
    firstCalculating = true;
    dX1 = dX2 = dY1 = dY2 = 0;
    newX1 = newX2 = newY1 = newY2 = 0;

    //apply numeric validators to line edits
    QRegExpValidator *numericValidator = new QRegExpValidator(QRegExp("\\d+"),this);
    ui->firstXLE->setValidator(numericValidator);
    ui->firstYLE->setValidator(numericValidator);
    ui->firstVelocityLE->setValidator(numericValidator);
    ui->firstAngleLE->setValidator(numericValidator);

    ui->secondXLE->setValidator(numericValidator);
    ui->secondYLE->setValidator(numericValidator);
    ui->secondVelocityLE->setValidator(numericValidator);
    ui->secondAngleLE->setValidator(numericValidator);

    if(!(emit haveLastData()))
    {
        ui->lastCalcPB->setEnabled(false);
    }

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(UpdateTime()));
    connect(ui->startPB,SIGNAL(clicked()),this,SLOT(startCalculating()));
}

//FOR TWO METHODS BELOW:
//
//Build the system of equations for coordinates
//  x = x0 + V * cosA * t
//  y = y0 + V * sinA * t
//
//  t = (x - x0) / V * cosA
//Express y in terms of x
//  y = y0 + V * sinA * ((x - x0)/V * cosA) = y0 + tgA * (x - x0)
//
//If paths are intersects y coordinates should be equals. Get another equal
//
// y01 + tgA(x - x01) = y02 + tgB(x - x02)
//
//After the transformations we can obtain the following formulas
//
//find X coordinate of intersection point
qreal MainWindow::findIntersectX(float initX1, float initX2, float initY1, float initY2, int AngleA, int AngleB)
{
   return (initY2 - initY1 + initX1*qTan(convertGradsToRadians(AngleA)) - initX2 * qTan(convertGradsToRadians(AngleB)))
          /(qTan(convertGradsToRadians(AngleA)) - qTan(convertGradsToRadians(AngleB)));
}

//find Y coordinate of intersection point
qreal MainWindow::findIntersectY(float initY, float intersectX, float initX, int Angle)
{
    return initY + qTan(convertGradsToRadians(Angle)) * (intersectX - initX);
}


//converting Angle to Radians from Degrees
qreal MainWindow::convertGradsToRadians(int Angle)
{
    return Angle * (PI/180);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Drawing pathways of moving objects
void MainWindow::drawLines(MovingObject *ob1,MovingObject *ob2)
{
    line1 = new QLineF(ob1->initX,zeroLine - ob1->initY,ob1->initX + this->width(),ob1->initY);
    line1->setAngle(ob1->Angle);
    scene->addLine(*line1,QPen(Qt::red));
    line2 = new QLineF(ob2->initX,zeroLine - ob2->initY,ob2->initX + this->width(),ob2->initY);
    line2->setAngle(ob2->Angle);
    scene->addLine(*line2,QPen(Qt::blue));
}

//Drawing and rotate to angle of moving objects (triangles)
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

//Calculating current coordinates for moving objects
QPointF MainWindow::calculateCurrentCoordinates(MovingObject *obj,qreal time)
{
    qreal currX = obj->currX + obj->Velocity * time * qCos(convertGradsToRadians(obj->Angle));
    qreal currY = obj->currY + obj->Velocity * time * qSin(convertGradsToRadians(obj->Angle));
    return QPointF(currX,currY);
}

//Drawing Boxes with current information (current coordinates, distance between moving object and intersection point
//,time that moving object needs to reach pathway of another moving object
void MainWindow::drawInfoBoxes(qreal X1, qreal Y1, qreal X2, qreal Y2, qreal Distance1, qreal Distance2, qreal Time1, qreal Time2)
{
    visualInfo *info1 = new visualInfo(QString::number(X1),QString::number(Y1),QString::number(Time1),QString::number(Distance1));
    info1->setPos(X1 + 50,zeroLine - Y1 + 40);
    scene->addItem(info1);

    visualInfo *info2 = new visualInfo(QString::number(X2),QString::number(Y2),QString::number(Time2),QString::number(Distance2));
    info2->setPos(X2 + 50,zeroLine - Y2 + 40);
    scene->addItem(info2);
}

//Definition of intersection object's paths. If intersects - get its coordinates
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

//Calculating of the distancebetween object's current coordinates and intersection point
qreal MainWindow::calculateDistance(qreal currentX, qreal currentY, qreal intersectX, qreal intersectY)
{
    return qSqrt(qPow((intersectX - currentX),2) + qPow((intersectY - currentY),2));
}


//Calculating of the time, that object needs to intersect second object's path
qreal MainWindow::calculateIntersectionTime(MovingObject *obj, qreal distance)
{
    return distance / obj->Velocity;
}

//Calculating all data after clicking on the start button
void MainWindow::startCalculating()
{
    scene->clear();
    intersectX = intersectY = 0;
    dX1 = dX2 = dY1 = dY2 = 0;

    timePastStart = 0;
    elapsedTime = new QTime(0,0);
    values_list.clear();
    objects_list.clear();

    isPaused = false;
    ui->pausePB->setText("Пауза");
    setLineEditsState(false);

    ui->TimeL->setText("00:00:00");
    ui->lastCalcPB->setEnabled(true);

    if(firstCalculating)
    {
        firstX1 = ui->firstXLE->text().toDouble();
        firstX2 = ui->secondXLE->text().toDouble();
        firstY1 = ui->firstYLE->text().toDouble();
        firstY2 = ui->secondYLE->text().toDouble();
        firstAngle1 = ui->firstAngleLE->text().toInt();
        firstAngle2 = ui->secondAngleLE->text().toInt();
        firstVelocity1 = ui->firstVelocityLE->text().toInt();
        firstVelocity2 = ui->secondVelocityLE->text().toInt();
    }
    firstCalculating = false;

    setLineEditsState(false);

    if(ui->firstXLE->text()!= "" && ui->firstYLE->text() != "" && ui->firstVelocityLE->text() != "" &&
            ui->firstAngleLE->text() != "" && ui->secondXLE->text() != "" && ui->secondYLE->text() != "" &&
            ui->secondVelocityLE->text() != "" && ui->secondAngleLE->text() != "")
    {
        qreal firstX = firstX1;
        qreal firstY = firstY1;
        int firstVelocity = firstVelocity1;
        int firstAngle = firstAngle1;

        qreal secondX = firstX2;
        qreal secondY = firstY2;
        int secondVelocity = firstVelocity2;
        int secondAngle = firstAngle2;

        values_list.append(QString::number(firstX));
        values_list.append(QString::number(firstY));
        values_list.append(QString::number(secondX));
        values_list.append(QString::number(secondY));
        values_list.append(QString::number(firstAngle));
        values_list.append(QString::number(firstVelocity));
        values_list.append(QString::number(secondAngle));
        values_list.append(QString::number(secondVelocity));

        emit InsertValuesSignal(values_list);

        obj1 = new MovingObject(firstX,firstY,firstAngle,firstVelocity);
        obj2 = new MovingObject(secondX,secondY,secondAngle,secondVelocity);

        drawLines(obj1,obj2);
        drawObjects(obj1->Angle,obj2->Angle,obj1->initX,obj1->initY,obj2->initX,obj2->initY);
        calculateIntersection(obj1,obj2);

        QPointF* intersectionPoint = new QPointF(intersectX,intersectY);

        if(intersectX && intersectY && (line1->intersect(*line2,intersectionPoint) == 1))
        {
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

//Calculate base data every second (1000 ms)
void MainWindow::UpdateTime()
{
    scene->clear();
    timePastStart++;
    objects_list.clear();
    calculateIntersection(obj1,obj2);
    scene->addEllipse(intersectX - 2.5,zeroLine - intersectY - 2.5,5,5,QPen(Qt::black),QBrush(Qt::black));
    drawLines(obj1,obj2);

    QPointF coordsObj1 = calculateCurrentCoordinates(obj1,1);
    QPointF coordsObj2 = calculateCurrentCoordinates(obj2,1);

    obj1->currX = coordsObj1.x();
    obj1->currY = coordsObj1.y();
    obj2->currX = coordsObj2.x();
    obj2->currY = coordsObj2.y();

    qreal DistanceObj1 = calculateDistance(obj1->currX,obj1->currY,intersectX,intersectY);
    qreal IntersectTimeObj1 = calculateIntersectionTime(obj1,DistanceObj1);

    qreal DistanceObj2 = calculateDistance(obj2->currX,obj2->currY,intersectX,intersectY);
    qreal IntersectTimeObj2 = calculateIntersectionTime(obj2,DistanceObj2);

    drawObjects(obj1->Angle,obj2->Angle,obj1->currX,obj1->currY,obj2->currX,obj2->currY);
    drawInfoBoxes(obj1->currX,obj1->currY,obj2->currX,obj2->currY,DistanceObj1,DistanceObj2,IntersectTimeObj1,IntersectTimeObj2);

    *elapsedTime = elapsedTime->addSecs(1);
    ui->TimeL->setText(elapsedTime->toString("hh:mm:ss"));

    updateUI();

    objects_list.append(QString::number(obj1->currX));
    objects_list.append(QString::number(obj1->currY));
    objects_list.append(QString::number(obj2->currX));
    objects_list.append(QString::number(obj2->currY));
    objects_list.append(QString::number(IntersectTimeObj1));
    objects_list.append(QString::number(DistanceObj1));
    objects_list.append(QString::number(IntersectTimeObj2));
    objects_list.append(QString::number(DistanceObj2));
    objects_list.append(QString::number(intersectX));
    objects_list.append(QString::number(intersectY));

    emit UpdateLastVal();
    emit InsertObjectsSignal(objects_list,values_list);
}

void MainWindow::updateUI()
{
    ui->firstXLE->setText(QString::number(obj1->currX));
    ui->firstYLE->setText(QString::number(obj1->currY));
    ui->secondXLE->setText(QString::number(obj2->currX));
    ui->secondYLE->setText(QString::number(obj2->currY));
}

void MainWindow::on_pausePB_clicked()
{
    isPaused = !isPaused;
    pauseResumeMethod(isPaused);
}

void MainWindow::on_stopPB_clicked()
{
   firstCalculating = true;
   timer->stop();
   timePastStart = 0;
   dX1 = dX2 = dY1 = dY2 = 0;
   intersectX = intersectY = 0;
   setLineEditsState(true);
   ui->TimeL->setText("00:00:00");
   ui->firstXLE->setText("");
   ui->firstYLE->setText("");
   ui->firstAngleLE->setText("");
   ui->firstVelocityLE->setText("");
   ui->secondXLE->setText("");
   ui->secondYLE->setText("");
   ui->secondAngleLE->setText("");
   ui->secondVelocityLE->setText("");
   ui->intersectionXLE->setText("");
   ui->intersectionYLE->setText("");
   scene->clear();
   emit UpdateLastVal();
   ui->lastCalcPB->setEnabled(false);
   disconnect(ui->firstXLE,SIGNAL(editingFinished()),this,SLOT(firstXchangedInPause()));
   disconnect(ui->firstYLE,SIGNAL(editingFinished()),this,SLOT(firstYchangedInPause()));
   disconnect(ui->secondXLE,SIGNAL(editingFinished()),this,SLOT(secondXchangedInPause()));
   disconnect(ui->secondYLE,SIGNAL(editingFinished()),this,SLOT(secondYchangedInPause()));
   disconnect(ui->firstVelocityLE,SIGNAL(editingFinished()),this,SLOT(firstSpeedChanged()));
   disconnect(ui->secondVelocityLE,SIGNAL(editingFinished()),this,SLOT(secondSpeedChanged()));
   disconnect(ui->firstAngleLE,SIGNAL(editingFinished()),this,SLOT(firstAngleChanged()));
   disconnect(ui->secondAngleLE,SIGNAL(editingFinished()),this,SLOT(secondAngleChanged()));
}

void MainWindow::firstXchangedInPause()
{
    dX1 = ui->firstXLE->text().toDouble() - obj1->currX;
    dY1 = dX1 * qTan(convertGradsToRadians(obj1->Angle));
    ui->firstYLE->setText("auto");
    obj1->initX = ui->firstXLE->text().toDouble();
    values_list[0] = ui->firstXLE->text();
    emit UpdateValuesSignal(ui->firstXLE->text(),0);
}

void MainWindow::firstYchangedInPause()
{
    dY1 = ui->firstYLE->text().toDouble() - obj1->currY;
    dX1 = dY1 / qTan(convertGradsToRadians(obj1->Angle));
    ui->firstXLE->setText("auto");
    obj1->initY = ui->firstYLE->text().toDouble();
    values_list[1] = ui->firstYLE->text();
    emit UpdateValuesSignal(ui->firstYLE->text(),1);
}

void MainWindow::secondXchangedInPause()
{
    dX2 = ui->secondXLE->text().toDouble() - obj2->currX;
    dY2 = dX2 * qTan(convertGradsToRadians(obj2->Angle));
    ui->secondYLE->setText("auto");
    obj2->initX = ui->secondXLE->text().toDouble();
    values_list[2] = ui->secondXLE->text();
    emit UpdateValuesSignal(ui->secondXLE->text(),2);
}

void MainWindow::secondYchangedInPause()
{
    dY2 = ui->secondYLE->text().toDouble() - obj2->currY;
    dX2 = dY2 / qTan(convertGradsToRadians(obj2->Angle));
    ui->secondXLE->setText("auto");
    obj2->initY = ui->secondYLE->text().toDouble();
    values_list[3] = ui->secondYLE->text();
    emit UpdateValuesSignal(ui->secondYLE->text(),3);
}

void MainWindow::firstSpeedChanged()
{
    obj1->Velocity = ui->firstVelocityLE->text().toInt();
    values_list[5] = ui->firstVelocityLE->text();
    emit UpdateValuesSignal(ui->firstVelocityLE->text(),5);
}

void MainWindow::secondSpeedChanged()
{
    obj2->Velocity = ui->secondVelocityLE->text().toInt();
    values_list[7] = ui->secondVelocityLE->text();
    emit UpdateValuesSignal(ui->secondVelocityLE->text(),7);
}

void MainWindow::firstAngleChanged()
{
    qreal Distance = calculateDistance(obj1->currX,obj1->currY,obj1->initX,obj1->initY);
    int newAngle = ui->firstAngleLE->text().toInt();
    dX1 = dY1 = 0;
    obj1->Angle = newAngle;
    obj1->currX = Distance * qCos(convertGradsToRadians(newAngle)) + obj1->initX;
    obj1->currY = Distance * qSin(convertGradsToRadians(newAngle)) + obj1->initY;
    ui->firstYLE->setText("auto");
    ui->firstXLE->setText("auto");
    values_list[4] = ui->firstAngleLE->text();
    emit UpdateValuesSignal(ui->firstAngleLE->text(),4);
}

void MainWindow::secondAngleChanged()
{
    qreal Distance = calculateDistance(obj2->currX,obj2->currY,obj2->initX,obj2->initY);
    int newAngle = ui->secondAngleLE->text().toInt();
    dX2 = dY2 = 0;
    obj2->Angle = newAngle;
    obj2->currX = Distance * qCos(convertGradsToRadians(newAngle)) + obj2->initX;
    obj2->currY = Distance * qSin(convertGradsToRadians(newAngle)) + obj2->initY;
    ui->secondYLE->setText("auto");
    ui->secondXLE->setText("auto");
    values_list[6] = ui->secondAngleLE->text();
    emit UpdateValuesSignal(ui->secondAngleLE->text(),6);
}

void MainWindow::pauseResumeMethod(bool value)
{
    if(value)
    {
        ui->pausePB->setText("Продолжить");
        //timer->blockSignals(true);
        timer->stop();
        setLineEditsState(true);
        connect(ui->firstXLE,SIGNAL(editingFinished()),this,SLOT(firstXchangedInPause()));
        connect(ui->firstYLE,SIGNAL(editingFinished()),this,SLOT(firstYchangedInPause()));
        connect(ui->secondXLE,SIGNAL(editingFinished()),this,SLOT(secondXchangedInPause()));
        connect(ui->secondYLE,SIGNAL(editingFinished()),this,SLOT(secondYchangedInPause()));
        connect(ui->firstVelocityLE,SIGNAL(editingFinished()),this,SLOT(firstSpeedChanged()));
        connect(ui->secondVelocityLE,SIGNAL(editingFinished()),this,SLOT(secondSpeedChanged()));
        connect(ui->firstAngleLE,SIGNAL(editingFinished()),this,SLOT(firstAngleChanged()));
        connect(ui->secondAngleLE,SIGNAL(editingFinished()),this,SLOT(secondAngleChanged()));
    }
    else{
        obj1->currX += dX1;
        obj1->currY += dY1;
        obj2->currX += dX2;
        obj2->currY += dY2;
        ui->pausePB->setText("Пауза");
        //timer->blockSignals(false);
        timer->start(1000);
        setLineEditsState(false);
        disconnect(ui->firstXLE,SIGNAL(editingFinished()),this,SLOT(firstXchangedInPause()));
        disconnect(ui->firstYLE,SIGNAL(editingFinished()),this,SLOT(firstYchangedInPause()));
        disconnect(ui->secondXLE,SIGNAL(editingFinished()),this,SLOT(secondXchangedInPause()));
        disconnect(ui->secondYLE,SIGNAL(editingFinished()),this,SLOT(secondYchangedInPause()));
        disconnect(ui->firstVelocityLE,SIGNAL(editingFinished()),this,SLOT(firstSpeedChanged()));
        disconnect(ui->secondVelocityLE,SIGNAL(editingFinished()),this,SLOT(secondSpeedChanged()));
        disconnect(ui->firstAngleLE,SIGNAL(editingFinished()),this,SLOT(firstAngleChanged()));
        disconnect(ui->secondAngleLE,SIGNAL(editingFinished()),this,SLOT(secondAngleChanged()));
    }
}

void MainWindow::setLineEditsState(bool state)
{
    ui->firstXLE->setEnabled(state);
    ui->firstYLE->setEnabled(state);
    ui->firstVelocityLE->setEnabled(state);
    ui->firstAngleLE->setEnabled(state);

    ui->secondXLE->setEnabled(state);
    ui->secondYLE->setEnabled(state);
    ui->secondVelocityLE->setEnabled(state);
    ui->secondAngleLE->setEnabled(state);
}

void MainWindow::on_lastCalcPB_clicked()
{
    QStringList list = emit getLastData();

    QString firstX = list[0];
    QString firstY = list[1];
    QString secondX = list[2];
    QString secondY = list[3];
    QString firstT = list[4];
    QString firstD = list[5];
    QString secondT = list[6];
    QString secondD = list[7];
    QString SintersectX = list[8];
    QString SintersectY = list[9];
    QString firstInitX = list[10];
    QString firstInitY = list[11];
    QString secondInitX = list[12];
    QString secondInitY = list[13];
    QString firstAngle = list[14];
    QString firstV = list[15];
    QString secondAngle = list[16];
    QString secondV = list[17];

    scene->clear();
    intersectX = intersectY = 0;
    dX1 = dX2 = dY1 = dY2 = 0;

    timePastStart = 0;
    elapsedTime = new QTime(0,0);
    isPaused = false;
    ui->pausePB->setText("Пауза");
    setLineEditsState(false);

    obj1 = new MovingObject();
    obj2 = new MovingObject();

    obj1->initX = firstInitX.toFloat();
    obj1->initY = firstInitY.toFloat();
    obj1->currX = firstX.toDouble();
    obj1->currY = firstY.toDouble();
    obj1->Angle = firstAngle.toInt();
    obj1->Velocity = firstV.toInt();

    obj2->initX = secondInitX.toFloat();
    obj2->initY = secondInitY.toFloat();
    obj2->currX = secondX.toDouble();
    obj2->currY = secondY.toDouble();
    obj2->Angle = secondAngle.toInt();
    obj2->Velocity = secondV.toInt();

    intersectX = SintersectX.toDouble();
    intersectY = SintersectY.toDouble();

    values_list.clear();
    values_list.append(firstInitX);
    values_list.append(firstInitY);
    values_list.append(secondInitX);
    values_list.append(secondInitY);
    values_list.append(firstAngle);
    values_list.append(firstV);
    values_list.append(secondAngle);
    values_list.append(secondV);

    timer->start(1000);
}
