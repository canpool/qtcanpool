#
#  QCustomPlot Plot Examples
#
include(../../../qtproject.pri)

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11
lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c++11

TARGET = plotdemo
TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

DEFINES += QCUSTOMPLOT_USE_LIBRARY

include($$QTCANPOOL_DIR/src/rpath.pri)

# lib
LIBS *= -l$$qtLibraryNameVersion(qcustomplot, 2)

SOURCES += main.cpp \
           mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

