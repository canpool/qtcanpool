# Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
# Copyright (C) 2021-2023 Antonio Dias (https://github.com/antonypro)
# SPDX-License-Identifier: MIT

QTC_LIB_DEPENDS += qgood qgoodbye
include(../../../qtproject.pri)

QT += core gui widgets

win32:equals(QT_MAJOR_VERSION, 5){
QT += winextras
}

equals(QT_MAJOR_VERSION, 6){
QT += openglwidgets
win32:LIBS += -lopengl32
}

CONFIG += c++11
DESTDIR = $$IDE_APP_PATH

equals(QT_MAJOR_VERSION, 5){
    DEFINES += QT_VERSION_QT5
}
equals(QT_MAJOR_VERSION, 6) {
    DEFINES += QT_VERSION_QT6
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    glwidget.cpp

HEADERS += \
    mainwindow.h \
    glwidget.h
