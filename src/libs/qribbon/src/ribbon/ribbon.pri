

INCLUDEPATH += $$quote($$PWD../../include)

SOURCES += \
    $$PWD/QtnRibbonSliderPane.cpp \
    $$PWD/QtnRibbonStatusBar.cpp \
    $$PWD/QtnRibbonTabBar.cpp \
    $$PWD/QtnRibbonSystemPopupBar.cpp \
    $$PWD/QtnRibbonPage.cpp \
    $$PWD/QtnRibbonGroupLayout.cpp \
    $$PWD/QtnRibbonGroup.cpp \
    $$PWD/QtnRibbonBar.cpp \
    $$PWD/QtnRibbonPrivate.cpp \
    $$PWD/QtnRibbonMainWindow.cpp \
    $$PWD/QtnRibbonButton.cpp \
    $$PWD/QtnRibbonToolTip.cpp \
    $$PWD/QtnRibbonQuickAccessBar.cpp \
    $$PWD/QtnOfficePopupColorButton.cpp \
    $$PWD/QtnRibbonGallery.cpp \
    $$PWD/QtnOfficePopupMenu.cpp \
    $$PWD/QtnRibbonBackstageView.cpp \
    $$PWD/QtnOfficePopupWindow.cpp \

HEADERS += \
    $$PWD/QtnOfficeDefines.h \
    $$PWD/QtnRibbonSliderPane.h \
    $$PWD/QtnRibbonStatusBar.h \
    $$PWD/QtnRibbonTabBar.h \
    $$PWD/QtnRibbonSystemPopupBar.h \
    $$PWD/QtnRibbonPage.h \
    $$PWD/QtnRibbonGroupLayout.h \
    $$PWD/QtnRibbonGroup.h \
    $$PWD/QtnRibbonBar.h \
    $$PWD/QtnRibbonBarPrivate.h \
    $$PWD/QtnRibbonGroupPrivate.h \
    $$PWD/QtnRibbonPrivate.h \
    $$PWD/QtnRibbonMainWindow.h \
    $$PWD/QtnRibbonButton.h \
    $$PWD/QtnRibbonToolTip.h \
    $$PWD/QtnRibbonQuickAccessBar.h \
    $$PWD/QtnOfficePopupColorButton.h \
    $$PWD/QtnRibbonGallery.h \
    $$PWD/QtnRibbonGalleryPrivate.h \
    $$PWD/QtnOfficePopupMenu.h \
    $$PWD/QtnRibbonBackstageView.h \
    $$PWD/QtnRibbonBackstageViewPrivate.h \
    $$PWD/QtnOfficePopupWindow.h

win32 {

    SOURCES += \
        $$PWD/QtnOfficeFrameHelper_win.cpp
    
    HEADERS += \
        $$PWD/QtnOfficeFrameHelper_win.h
}

unix {
    SOURCES += 
}

RESOURCES += \
    $$PWD/QtnRibbonResources.qrc


win32 {
    DEFINES += _WINDOWS
    LIBS += -luser32 -lgdi32 -lshell32
}
