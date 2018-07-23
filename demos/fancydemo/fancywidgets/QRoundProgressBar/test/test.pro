#-------------------------------------------------
#
# Project created by QtCreator 2015-05-08T23:32:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QRoundProgressBarTest
TEMPLATE = app


SOURCES += main.cpp\
        testwidget.cpp \
    ../src/QRoundProgressBar.cpp

HEADERS  += testwidget.h \
    ../src/QRoundProgressBar.h

INCLUDEPATH += ../src

FORMS    += testwidget.ui
