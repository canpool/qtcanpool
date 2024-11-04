VERSION = 0.0.1

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QX_DOCK_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

PUBLIC_HEADERS = \
    $$PWD/qxdock_global.h \
    $$PWD/dockcontainer.h \
    $$PWD/dockpanel.h \
    $$PWD/dockwidget.h \
    $$PWD/dockwindow.h \
    $$PWD/docksplitter.h \
    $$PWD/dockoverlay.h

PRIVATE_HEADERS = \

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

SOURCES += \
    $$PWD/dockcontainer.cpp \
    $$PWD/dockpanel.cpp \
    $$PWD/dockwidget.cpp \
    $$PWD/dockwindow.cpp \
    $$PWD/docksplitter.cpp \
    $$PWD/dockoverlay.cpp

RESOURCES += \
