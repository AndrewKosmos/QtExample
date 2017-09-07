#include <QApplication>
#include <QTextCodec>
#include "databaseworker.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);

    DatabaseWorker dbw("data.db");

    MainWindow w;
    QObject::connect(&w,SIGNAL(InsertValuesSignal(QStringList)),&dbw,SLOT(InsertValues(QStringList)));
    QObject::connect(&w,SIGNAL(UpdateLastVal()),&dbw,SLOT(DeleteSessionValues()));
    QObject::connect(&w,SIGNAL(UpdateValuesSignal(QString,int)),&dbw,SLOT(UpdateValues(QString,int)));
    QObject::connect(&w,SIGNAL(InsertObjectsSignal(QStringList,QStringList)),&dbw,SLOT(InsertObjects(QStringList,QStringList)));
    QObject::connect(&w,SIGNAL(getLastData()),&dbw,SLOT(GetLastData()));
    QObject::connect(&w,SIGNAL(haveLastData()),&dbw,SLOT(haveLastData()));
    w.show();
    
    return a.exec();
}
