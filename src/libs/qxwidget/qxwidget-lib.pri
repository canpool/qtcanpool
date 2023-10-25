VERSION = 0.1.0

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QX_WIDGET_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    $$PWD/extensionbutton.h \
    $$PWD/menuaccessbutton.h \
    $$PWD/menubutton.h \
    $$PWD/qxwidget_global.h \
    $$PWD/tinynavbar.h \
    $$PWD/tinynavbar_p.h \
    $$PWD/tinytabbar.h \
    $$PWD/tinytabbar_p.h \
    $$PWD/tinytabwidget.h \
    $$PWD/windowlogo.h \
    $$PWD/windowtoolbar.h \
    $$PWD/windowtoolbar_p.h

SOURCES += \
    $$PWD/extensionbutton.cpp \
    $$PWD/menuaccessbutton.cpp \
    $$PWD/menubutton.cpp \
    $$PWD/tinynavbar.cpp \
    $$PWD/tinytabbar.cpp \
    $$PWD/tinytabwidget.cpp \
    $$PWD/windowlogo.cpp \
    $$PWD/windowtoolbar.cpp
