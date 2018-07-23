#-------------------------------------------------
#
# Project created by QtCreator 2017-02-10T17:31:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DynamicProgressBar
TEMPLATE = app


SOURCES += main.cpp\
    dynamicprogressbartest.cpp \
    taichiwait.cpp \
    circlewait.cpp \
    piewait.cpp \
    linewait.cpp \
    donutwait.cpp \
    zoomcirclewait.cpp

HEADERS  += dynamicprogressbar.h \
    dynamicprogressbartest.h \
    taichiwait.h \
    circlewait.h \
    piewait.h \
    linewait.h \
    donutwait.h \
    zoomcirclewait.h

FORMS += \
    dynamicprogressbartest.ui

MOC_DIR = temp/moc
RCC_DIR = temp/rcc
UI_DIR = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR = bin
