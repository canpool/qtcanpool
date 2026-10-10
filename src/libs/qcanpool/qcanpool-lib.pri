VERSION = 2.0.2

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

# 3.1 emptied this library: the general-purpose widgets moved to qxapp (A1)
# and the window-chrome family was retired (A2). What is listed now are the
# deprecated forwarding classes that keep 3.0 code compiling for one release,
# plus the resources that are still part of the published surface. The names
# disappear in 3.2, together with the library.
PUBLIC_HEADERS = \
    $$PWD/extensionbutton.h \
    $$PWD/fancytoolbutton.h \
    $$PWD/menuaccessbutton.h \
    $$PWD/menubutton.h \
    $$PWD/qcanpool.h \
    $$PWD/tinynavbar.h \
    $$PWD/tinytabbar.h \
    $$PWD/tinytabwidget.h

PRIVATE_HEADERS = \


SOURCES += \

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

RESOURCES += \
    $$PWD/qcanpool.qrc
