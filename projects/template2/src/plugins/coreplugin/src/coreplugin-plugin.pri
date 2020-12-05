DEFINES += CORE_LIBRARY

QT += \
    printsupport \

HEADERS += \
    $$PWD/core_global.h \
    $$PWD/coreplugin.h \
    $$PWD/mainwindow.h \
    $$PWD/coreconstants.h \
    $$PWD/coreicons.h

SOURCES += \
    $$PWD/coreplugin.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/coreicons.cpp


RESOURCES += \
    $$PWD/core.qrc
