#-------------------------------------------------
#
# Project created by QtCreator 2017-03-24T09:46:55
#
#-------------------------------------------------
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QCANPOOL_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TRANSLATIONS += resource/qm/qcanpool_zh_CN.ts

HEADERS += \
    $$PWD/qcanpool_global.h \
    $$PWD/imode.h \
    $$PWD/fancytabwidget.h \
    $$PWD/fancytabbar.h \
    $$PWD/fancytab.h \
    $$PWD/modemanager.h \
    $$PWD/minisplitter.h \
    $$PWD/simplefilter.h \
    $$PWD/minicard.h \
    $$PWD/ministack.h \
    $$PWD/qcanpool.h \
    $$PWD/centralarea.h \
    $$PWD/tinytabwidget.h \
    $$PWD/fancybar.h \
    $$PWD/quickaccessbar.h \
    $$PWD/fancybutton.h \
    $$PWD/cursor.h \
    $$PWD/screenhelper.h \
    $$PWD/fancywindow.h \
    $$PWD/fancydialog.h \
    $$PWD/skinitem.h \
    $$PWD/fancynavbar.h \
    $$PWD/fancynavbutton.h \
    $$PWD/imodebar.h \
    $$PWD/imodebar_p.h \
    $$PWD/imodebutton.h \
    $$PWD/pagesorter.h \
    $$PWD/fancybanner.h \
    $$PWD/qcanpool_p.h

SOURCES += \
    $$PWD/imode.cpp \
    $$PWD/fancytabwidget.cpp \
    $$PWD/fancytabbar.cpp \
    $$PWD/fancytab.cpp \
    $$PWD/modemanager.cpp \
    $$PWD/minisplitter.cpp \
    $$PWD/simplefilter.cpp \
    $$PWD/minicard.cpp \
    $$PWD/ministack.cpp \
    $$PWD/qcanpool.cpp \
    $$PWD/centralarea.cpp \
    $$PWD/tinytabwidget.cpp \
    $$PWD/fancybar.cpp \
    $$PWD/quickaccessbar.cpp \
    $$PWD/fancybutton.cpp \
    $$PWD/cursor.cpp \
    $$PWD/screenhelper.cpp \
    $$PWD/fancywindow.cpp \
    $$PWD/fancydialog.cpp \
    $$PWD/skinitem.cpp \
    $$PWD/fancynavbar.cpp \
    $$PWD/fancynavbutton.cpp \
    $$PWD/imodebar.cpp \
    $$PWD/imodebutton.cpp \
    $$PWD/pagesorter.cpp \
    $$PWD/fancybanner.cpp \
    $$PWD/qcanpool_p.cpp

RESOURCES += \
    $$PWD/qcanpool.qrc

