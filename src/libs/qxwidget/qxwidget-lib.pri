VERSION = 0.0.1

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QX_WIDGET_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

PUBLIC_HEADERS = \
    $$PWD/qxwidget_global.h

PRIVATE_HEADERS = \


HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

SOURCES += \


RESOURCES += \
