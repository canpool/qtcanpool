QTC_LIB_DEPENDS += qcanpool
include(../../../../qtproject.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TARGET = tst_qcanpool_inherit_fancywidget
DESTDIR = $$IDE_APP_PATH

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp

HEADERS += \
    $$PWD/mainwindow.h
