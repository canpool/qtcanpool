QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QCANPOOL_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
    DEFINES += QTC_USE_NATIVE
    LIBS += -lUser32
}

HEADERS += \
    $$PWD/qcanpool.h \
    $$PWD/quickaccessbar.h \
    $$PWD/quickaccessbar_p.h \
    $$PWD/ribbonbar.h \
    $$PWD/ribbonbar_p.h \
    $$PWD/ribboncontainers.h \
    $$PWD/ribbongroup.h \
    $$PWD/ribbongroup_p.h \
    $$PWD/ribbonpage.h \
    $$PWD/ribbonpage_p.h \
    $$PWD/ribbonwindow.h

SOURCES += \
    $$PWD/quickaccessbar.cpp \
    $$PWD/ribbonbar.cpp \
    $$PWD/ribboncontainers.cpp \
    $$PWD/ribbongroup.cpp \
    $$PWD/ribbonpage.cpp \
    $$PWD/ribbonwindow.cpp

include($$PWD/fancy.pri)

RESOURCES += \
    $$PWD/qcanpool.qrc

TRANSLATIONS += resource/qm/qcanpool_zh_CN.ts
