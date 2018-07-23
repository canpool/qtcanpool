#-------------------------------------------------
#
# Project created by QtCreator 2017-02-09T16:23:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ColorProgressBar
TEMPLATE = app

INCLUDEPATH += ../src

SOURCES += main.cpp\
        ../src/colorprogressbar.cpp \
    colorprogressbartest.cpp

HEADERS  += ../src/colorprogressbar.h \
    colorprogressbartest.h

FORMS += \
    colorprogressbartest.ui

#MOC_DIR = temp/moc
RCC_DIR = temp/rcc
#UI_DIR = temp/ui
#OBJECTS_DIR = temp/obj
DESTDIR = bin
