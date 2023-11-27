# Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
# Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
# SPDX-License-Identifier: MIT

QTC_LIB_DEPENDS += qgood
include(../../../qtproject.pri)

QT += core gui widgets

CONFIG += c++11
DESTDIR = $$IDE_APP_PATH

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h
