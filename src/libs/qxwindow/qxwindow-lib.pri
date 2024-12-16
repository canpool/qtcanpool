VERSION = 0.1.1

CONFIG += c++17

QT += widgets

equals(QX_WINDOW_CONFIG_BUILD_STATIC, 1) {
    DEFINES += QX_WINDOW_LIBRARY_STATIC
} else {
    DEFINES += QX_WINDOW_LIBRARY
}

equals(QX_WINDOW_CONFIG_NATIVE, 1) {
    DEFINES += QX_WINDOW_NATIVE
}
equals(QX_WINDOW_CONFIG_ENABLE_SYSTEM_BORDERS, 1) {
    DEFINES += QX_WINDOW_ENABLE_SYSTEM_BORDERS
}
equals(QX_WINDOW_CONFIG_ENABLE_STYLE_AGENT, 1) {
    DEFINES += QX_WINDOW_ENABLE_STYLE_AGENT
}

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    $$PWD/qxwindow_global.h \
    $$PWD/windowagentbase.h \
    $$PWD/windowagentbase_p.h \
    $$PWD/windowcontext_p.h \
    $$PWD/windowevent_p.h \
    $$PWD/windowitemdelegate_p.h \
    $$PWD/windowkit_p.h \
    $$PWD/windowsystem_p.h

SOURCES += \
    $$PWD/windowagentbase.cpp \
    $$PWD/windowcontext.cpp \
    $$PWD/windowevent.cpp \
    $$PWD/windowitemdelegate.cpp

equals(QX_WINDOW_CONFIG_NATIVE, 1) {

    QT +=  core-private gui-private

    win32 {
        HEADERS += \
            $$PWD/windowkit_win.h \
            $$PWD/windowkit_win_p.h \

        SOURCES += \
            $$PWD/windowkit_win.cpp \
            $$PWD/windowcontext_win.cpp \

        LIBS += -lUser32 -lShell32 -lGdi32
    }
    linux* {
        HEADERS += \
            $$PWD/windowkit_linux.h \

        SOURCES += \
            $$PWD/windowcontext_qt.cpp \

    }
    macx {
        SOURCES += \
            $$PWD/windowcontext_cocoa.mm \

    }
} else {
    SOURCES += \
        $$PWD/windowcontext_qt.cpp \

}

!equals(QX_WINDOW_CONFIG_WIDGET_DISABLE, 1) {
    HEADERS += \
        $$PWD/widgetitemdelegate_p.h \
        $$PWD/windowagentwidget.h \
        $$PWD/windowagentwidget_p.h \

    SOURCES += \
        $$PWD/widgetitemdelegate.cpp \
        $$PWD/windowagentwidget.cpp \

    equals(QX_WINDOW_CONFIG_NATIVE, 1) {
        win32 {
            SOURCES += \
                $$PWD/windowagentwidget_win.cpp \

        }
        macx {
            SOURCES += \
                $$PWD/windowagentwidget_mac.cpp \

        }
    }
}

equals(QX_WINDOW_CONFIG_ENABLE_STYLE_AGENT, 1) {
    include($$PWD/style/style.pri)
}
