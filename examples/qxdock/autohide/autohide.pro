QTC_LIB_DEPENDS += qxdock
include(../shared/shared.pri)

TARGET = Example_Dock_AutoHide

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/mainwindow.h

SOURCES += \
    $$PWD/mainwindow.cpp

FORMS += \
    $$PWD/mainwindow.ui
