INCLUDEPATH += $$PWD/../ribbon

ALL_SOURCES = \
    $$PWD/QtnCommonStyle.cpp \
    $$PWD/QtnOfficeStyle.cpp \
    $$PWD/QtnRibbonStyle.cpp \
    $$PWD/QtnStyleHelpers.cpp \
    $$PWD/QtnPopupHelpers.cpp \
    $$PWD/QtnCommonPaintManager.cpp \
    $$PWD/QtnOfficePaintManager.cpp \
    $$PWD/QtnRibbonPaintManager.cpp \

INCLUDE_HEADERS = \
    $$PWD/../../include/QtnCommonStyle.h \
    $$PWD/../../include/QtnOfficeDefines.h \
    $$PWD/../../include/QtnOfficePopupColorButton.h \
    $$PWD/../../include/QtnOfficePopupMenu.h \
    $$PWD/../../include/QtnOfficePopupWindow.h \
    $$PWD/../../include/QtnOfficeStyle.h \
    $$PWD/../../include/QtnRibbonStyle.h \
   $$PWD/../../include/QtnStyleHelpers.h

PRIVATE_HEADERS = \
    $$PWD/QtnCommonStylePrivate.h \
    $$PWD/QtnOfficeStylePrivate.h \
    $$PWD/QtnRibbonStylePrivate.h

PUBLIC_HEADERS = \
    $$PWD/QtnCommonStyle.h \
    $$PWD/QtnOfficeStyle.h \
    $$PWD/QtnRibbonStyle.h \
    $$PWD/QtnStyleHelpers.h \
    $$PWD/QtnPopupHelpers.h

WINDOWS_HEADERS =
LINUX_HEADERS =
MACOSX_HEADERS =

WINDOWS_SOURCES = $$PWD/QtnCommonStyle_win.cpp
LINUX_SOURCES = $$PWD/QtnCommonStyle_linux.cpp
MACOSX_SOURCES = $$PWD/QtnCommonStyle_mac.mm



HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

win32:HEADERS += $$WINDOWS_HEADERS
linux*:HEADERS += $$LINUX_HEADERS
macx:HEADERS += $$MACOSX_HEADERS

SOURCES += \
    $$ALL_SOURCES

win32:SOURCES += $$WINDOWS_SOURCES
linux*:SOURCES += $$LINUX_SOURCES
macx:OBJECTIVE_SOURCES += $$MACOSX_SOURCES


