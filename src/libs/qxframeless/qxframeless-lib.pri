VERSION = 0.1.0

QT += widgets

equals(QX_FRAMELESS_CONFIG_BUILD_STATIC, 1) {
    DEFINES += QX_FRAMELESS_LIBRARY_STATIC
} else {
    DEFINES += QX_FRAMELESS_LIBRARY
}

equals(QX_FRAMELESS_CONFIG_NATIVE, 1) {
    DEFINES += QX_FRAMELESS_NATIVE
}

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/framelesshelper.h \
    $$PWD/framelesshelper_p.h \
    $$PWD/qxframeless_global.h

SOURCES += \
    $$PWD/framelesshelper.cpp

win32-msvc {
    LIBS += -lUser32
}
