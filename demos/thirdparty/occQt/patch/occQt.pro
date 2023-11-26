# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0

TOPDIR = ../../..

OPENCASCADE_LIBS = \
    TKernel     \
    TKMath      \
    TKG3d       \
    TKBRep      \
    TKGeomBase  \
    TKGeomAlgo  \
    TKTopAlgo   \
    TKPrim      \
    TKBO        \
    TKBool      \
    TKOffset    \
    TKService   \
    TKV3d       \
    TKOpenGl    \
    TKFillet

# dependecies
OPENCASCADE_LIBS += \
    TKG2d \
    TKShHealing \
    TKHLR \
    TKMesh \

# must after OPENCASCADE_LIBS
include($$TOPDIR/thirdparty/opencascade/opencascade.pri)

QT += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = occQt
TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

CONFIG += c++11

SOURCES += main.cpp \
    occQt.cpp       \
    occView.cpp

HEADERS  += \
    occQt.h \
    occView.h

FORMS    += \
    occQt.ui

RESOURCES += \
    occqt.qrc

win32 {
    DEFINES += WNT
}

# auto copy dll to STATIC_OUTPUT_BASE
STATIC_OUTPUT_BASE = $$DESTDIR
include($$TOPDIR/qtdata.pri)
