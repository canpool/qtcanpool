HEADERS += \
    $$PWD/windowstyleagent.h \
    $$PWD/windowstyleagent_p.h

SOURCES += \
    $$PWD/windowstyleagent.cpp \

win32 {
    SOURCES += \
        $$PWD/windowstyleagent_win.cpp \

}

linux* {
    SOURCES += \
        $$PWD/windowstyleagent_linux.cpp \

}

macx {
    SOURCES += \
        $$PWD/windowstyleagent_mac.cpp \

}
