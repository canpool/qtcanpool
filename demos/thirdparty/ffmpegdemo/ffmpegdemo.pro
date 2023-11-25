TOPDIR = ../../..
include($$TOPDIR/thirdparty/ffmpeg/ffmpeg.pri)

QT += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ffmpegdemo
TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

DEFINES += QT_DEPRECATED_WARNINGS

# auto copy dll to STATIC_OUTPUT_BASE
STATIC_OUTPUT_BASE = $$DESTDIR
include($$TOPDIR/qtdata.pri)

SOURCES += main.cpp
