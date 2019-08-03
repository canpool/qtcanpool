#-------------------------------------------------
#
# Project created by QtCreator 2017-03-24T09:46:55
#
#-------------------------------------------------
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QLITE_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

VERSION = 1.0.0

TRANSLATIONS += resource/qm/qlite_zh_CN.ts

RESOURCES += \
    $$PWD/qlite.qrc

HEADERS += \
    $$PWD/litebar.h \
    $$PWD/litebar_p.h \
    $$PWD/litedialog.h \
    $$PWD/litemodebar.h \
    $$PWD/litescreen.h \
    $$PWD/litetabbar.h \
    $$PWD/litetabwidget.h \
    $$PWD/litewidget.h \
    $$PWD/litewindow.h \
    $$PWD/qlite.h \
    $$PWD/qlite_global.h \
    $$PWD/liteboard.h

SOURCES += \
    $$PWD/litebar.cpp \
    $$PWD/litedialog.cpp \
    $$PWD/litemodebar.cpp \
    $$PWD/litescreen.cpp \
    $$PWD/litetabbar.cpp \
    $$PWD/litetabwidget.cpp \
    $$PWD/litewidget.cpp \
    $$PWD/litewindow.cpp \
    $$PWD/qlite.cpp \
    $$PWD/liteboard.cpp
