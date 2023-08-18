VERSION = 0.6.0

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QX_WIDGET_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

include($$PWD/qxwidget_base.pri)
include($$PWD/qxwidget_ribbon.pri)

