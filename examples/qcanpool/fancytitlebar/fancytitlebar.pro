QTC_LIB_DEPENDS += qcanpool
include(../../shared/shared.pri)

TARGET = Example_FancyTitleBar

SOURCES += \
    $$PWD/mainwindow.cpp

win32 {
    DEFINES += USE_NATIVE
    LIBS += -lUser32
}
