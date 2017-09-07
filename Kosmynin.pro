#-------------------------------------------------
#
# Project created by QtCreator 2017-07-30T00:41:56
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Kosmynin
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    movingobject.cpp \
    movingobjectvisual.cpp \
    visualinfo.cpp \
    databaseworker.cpp

HEADERS  += mainwindow.h \
    movingobject.h \
    movingobjectvisual.h \
    visualinfo.h \
    databaseworker.h

FORMS    += mainwindow.ui
