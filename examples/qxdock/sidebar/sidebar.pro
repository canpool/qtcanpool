QTC_LIB_DEPENDS += qxdock
include(../shared/shared.pri)

TARGET = Example_Dock_SideBar

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/mainwindow.h

SOURCES += \
    $$PWD/mainwindow.cpp

FORMS += \
    $$PWD/mainwindow.ui
