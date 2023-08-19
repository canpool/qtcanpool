HEADERS += \
    $$PWD/QxFramelessHelper.h \
    $$PWD/QxGlobal.h \
    $$PWD/QxMenuButton.h \
    $$PWD/QxQuickAccessBar.h \
    $$PWD/QxQuickAccessBarPrivate.h \
    $$PWD/QxTinyNavBar.h \
    $$PWD/QxTinyNavBarPrivate.h \
    $$PWD/QxTinyTabBar.h \
    $$PWD/QxTinyTabBarPrivate.h \
    $$PWD/QxTinyTabWidget.h \
    $$PWD/QxUtils.h

SOURCES += \
    $$PWD/QxFramelessHelper.cpp \
    $$PWD/QxMenuButton.cpp \
    $$PWD/QxQuickAccessBar.cpp \
    $$PWD/QxTinyNavBar.cpp \
    $$PWD/QxTinyTabBar.cpp \
    $$PWD/QxTinyTabWidget.cpp \
    $$PWD/QxUtils.cpp

win32 {
    # for FramelessHelper
    LIBS += -lUser32
}
