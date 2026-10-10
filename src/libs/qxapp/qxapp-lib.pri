VERSION = 0.0.1

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QX_APP_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

PUBLIC_HEADERS = \
    $$PWD/qxapp_global.h \
    $$PWD/qxextensionbutton.h \
    $$PWD/qxmenuaccessbutton.h \
    $$PWD/qxmenubutton.h \
    $$PWD/qxnavbar.h \
    $$PWD/qxtabbar.h \
    $$PWD/qxtabwidget.h \
    $$PWD/qxtoolbutton.h \
    $$PWD/ribbonappwindow.h

PRIVATE_HEADERS = \
    $$PWD/qxnavbar_p.h \
    $$PWD/qxtabbar_p.h


HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

SOURCES += \
    $$PWD/qxextensionbutton.cpp \
    $$PWD/qxmenuaccessbutton.cpp \
    $$PWD/qxmenubutton.cpp \
    $$PWD/qxnavbar.cpp \
    $$PWD/qxtabbar.cpp \
    $$PWD/qxtabwidget.cpp \
    $$PWD/qxtoolbutton.cpp \
    $$PWD/ribbonappwindow.cpp

RESOURCES += \
