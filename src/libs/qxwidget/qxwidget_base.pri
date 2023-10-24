HEADERS += \
    $$PWD/framelesshelper.h \
    $$PWD/menubutton.h \
    $$PWD/quickaccessbar.h \
    $$PWD/quickaccessbar_p.h \
    $$PWD/qxglobal.h \
    $$PWD/ribbonutils.h \
    $$PWD/tinynavbar.h \
    $$PWD/tinynavbar_p.h \
    $$PWD/tinytabbar.h \
    $$PWD/tinytabbar_p.h \
    $$PWD/tinytabwidget.h

SOURCES += \
    $$PWD/framelesshelper.cpp \
    $$PWD/menubutton.cpp \
    $$PWD/quickaccessbar.cpp \
    $$PWD/ribbonutils.cpp \
    $$PWD/tinynavbar.cpp \
    $$PWD/tinytabbar.cpp \
    $$PWD/tinytabwidget.cpp

win32 {
    # for FramelessHelper
    LIBS += -lUser32
}
