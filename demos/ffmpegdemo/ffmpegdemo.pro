#-------------------------------------------------
#
# Project created by QtCreator 2018-01-27T20:52:44
#
#-------------------------------------------------
TOPDIR = ../..
include($$TOPDIR/thirdparty/ffmpeg/ffmpeg.pri)

QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ffmpegdemo
TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# auto copy dll to STATIC_OUTPUT_BASE
STATIC_OUTPUT_BASE = $$DESTDIR
include($$TOPDIR/data.pri)


SOURCES += main.cpp

