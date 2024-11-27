QTC_LIB_DEPENDS += qxdock
include(../shared/shared.pri)

TARGET = Example_Dock_DockInDock

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/mainwindow.h

SOURCES += \
    $$PWD/mainwindow.cpp
