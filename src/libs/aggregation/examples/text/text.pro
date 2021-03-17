QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = text
TEMPLATE = app

INCLUDEPATH += ../../

include(../../aggregation-lib.pri)

SOURCES += main.cpp

HEADERS += main.h \
    myinterfaces.h

FORMS += main.ui
