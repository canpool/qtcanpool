QTC_LIB_DEPENDS += qxribbon
include(../shared/shared.pri)

TARGET = Example_Ribbon_Menu

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/mainwindow.h \
    $$PWD/colorbutton.h

SOURCES += \
    $$PWD/mainwindow.cpp \
    $$PWD/colorbutton.cpp
