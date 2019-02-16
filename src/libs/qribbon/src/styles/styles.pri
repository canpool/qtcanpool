
INCLUDEPATH += $$quote($$PWD/../../include)

SOURCES += \
    $$PWD/QtnCommonStyle.cpp \
    $$PWD/QtnOfficeStyle.cpp \
    $$PWD/QtnRibbonStyle.cpp \
    $$PWD/QtnStyleHelpers.cpp \
    $$PWD/QtnPopupHelpers.cpp \

HEADERS += \
    $$PWD/QtnCommonStyle.h \
    $$PWD/QtnCommonStylePrivate.h \
    $$PWD/QtnOfficeStyle.h \
    $$PWD/QtnRibbonStyle.h \
    $$PWD/QtnStyleHelpers.h \
    $$PWD/QtnPopupHelpers.h \


win32 {
    SOURCES += $$PWD/QtnCommonStyle_win.cpp
}

linux-g++|linux-g++-64 {
    SOURCES += $$PWD/QtnCommonStyle_linux.cpp
}

macx {
    OBJECTIVE_SOURCES += $$PWD/QtnCommonStyle_mac.mm
    LIBS += -framework AppKit
}
