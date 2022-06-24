	
QTN_COMPONENT_VERSION = 4.8.0
DEFINES += QTN_VERSION_RIBBON=$$QTN_COMPONENT_VERSION

INCLUDEPATH += $$quote($$PWD/../../include)
INCLUDEPATH += $$quote($$PWD/../styles)
INCLUDEPATH += $$quote($$PWD/../base)

ALL_SOURCES = \
    $$PWD/QtnRibbonSliderPane.cpp \
    $$PWD/QtnRibbonStatusBar.cpp \
    $$PWD/QtnRibbonTabBar.cpp \
    $$PWD/QtnRibbonSystemPopupBar.cpp \
    $$PWD/QtnRibbonPage.cpp \
    $$PWD/QtnRibbonGroup.cpp \
    $$PWD/QtnRibbonBar.cpp \
    $$PWD/QtnRibbonPrivate.cpp \
    $$PWD/QtnRibbonMainWindow.cpp \
    $$PWD/QtnRibbonButtonPrivate.cpp \
    $$PWD/QtnRibbonControls.cpp \
    $$PWD/QtnRibbonButtonControls.cpp \
    $$PWD/QtnRibbonInputControls.cpp \
    $$PWD/QtnRibbonGalleryControls.cpp \
    $$PWD/QtnRibbonToolTip.cpp \
    $$PWD/QtnRibbonQuickAccessBar.cpp \
    $$PWD/QtnOfficePopupColorButton.cpp \
    $$PWD/QtnRibbonGallery.cpp \
    $$PWD/QtnOfficePopupMenu.cpp \
    $$PWD/QtnRibbonBackstageView.cpp \
    $$PWD/QtnOfficePopupWindow.cpp \
    $$PWD/QtnRibbonWorkspace.cpp \
    $$PWD/QtnRibbonCustomizeDialog.cpp \
    $$PWD/QtnRibbonCustomizePage.cpp \
    $$PWD/QtnRibbonCustomizeManager.cpp \
    $$PWD/QtnRibbonDef.cpp

INCLUDE_HEADERS = \
    $$PWD/../../include/QtitanRibbon.h \
    $$PWD/../../include/QtitanDef.h \
    $$PWD/../../include/QtnRibbonDef.h \
    $$PWD/../../include/QtnRibbonBackstageView.h \
    $$PWD/../../include/QtnRibbonBar.h \
    $$PWD/../../include/QtnRibbonGallery.h \
    $$PWD/../../include/QtnRibbonGroup.h \
    $$PWD/../../include/QtnRibbonMainWindow.h \
    $$PWD/../../include/QtnRibbonPage.h \
    $$PWD/../../include/QtnRibbonQuickAccessBar.h \
    $$PWD/../../include/QtnRibbonSliderPane.h \
    $$PWD/../../include/QtnRibbonStatusBar.h \
    $$PWD/../../include/QtnRibbonSliderPane.h \
    $$PWD/../../include/QtnRibbonSystemPopupBar.h \
    $$PWD/../../include/QtnRibbonToolTip.h \
    $$PWD/../../include/QtnRibbonInputControls.h \
    $$PWD/../../include/QtnRibbonButtonControls.h \
    $$PWD/../../include/QtnRibbonGalleryControls.h \
    $$PWD/../../include/QtnRibbonCustomizeManager.h \
    $$PWD/../../include/QtnRibbonCustomizeDialog.h \
    $$PWD/../../include/QtnRibbonCustomizePage.h \
    $$PWD/../../include/QtnRibbonWorkspace.h

PRIVATE_HEADERS = \
    $$PWD/QtnRibbonBarPrivate.h \
    $$PWD/QtnRibbonPagePrivate.h \
    $$PWD/QtnRibbonGroupPrivate.h \
    $$PWD/QtnRibbonPrivate.h \
    $$PWD/QtnRibbonGalleryPrivate.h \
    $$PWD/QtnRibbonBackstageViewPrivate.h \
    $$PWD/QtnOfficeFrameHelper.h

PUBLIC_HEADERS = \
    $$PWD/QtnRibbonDef.h \
    $$PWD/QtnRibbonSliderPane.h \
    $$PWD/QtnRibbonStatusBar.h \
    $$PWD/QtnRibbonTabBar.h \
    $$PWD/QtnRibbonSystemPopupBar.h \
    $$PWD/QtnRibbonPage.h \
    $$PWD/QtnRibbonGroup.h \
    $$PWD/QtnRibbonBar.h \
    $$PWD/QtnRibbonMainWindow.h \
    $$PWD/QtnRibbonButtonPrivate.h \
    $$PWD/QtnRibbonControls.h \
    $$PWD/QtnRibbonControlsPrivate.h \
    $$PWD/QtnRibbonButtonControls.h \
    $$PWD/QtnRibbonInputControls.h \
    $$PWD/QtnRibbonGalleryControls.h \
    $$PWD/QtnRibbonToolTip.h \
    $$PWD/QtnRibbonQuickAccessBar.h \
    $$PWD/QtnOfficePopupColorButton.h \
    $$PWD/QtnRibbonGallery.h \
    $$PWD/QtnOfficePopupMenu.h \
    $$PWD/QtnRibbonBackstageView.h \
    $$PWD/QtnOfficePopupWindow.h \
    $$PWD/QtnRibbonWorkspace.h \
    $$PWD/QtnRibbonCustomizeDialog.h \
    $$PWD/QtnRibbonCustomizeDialogPrivate.h \
    $$PWD/QtnRibbonCustomizePage.h \
    $$PWD/QtnRibbonCustomizeManager.h \
    $$PWD/QtnRibbonCustomizeManagerPrivate.h \

WINDOWS_SOURCES = $$PWD/QtnOfficeFrameHelper_win.cpp
LINUX_SOURCES =
MACOSX_SOURCES =

    
WINDOWS_HEADERS = $$PWD/QtnOfficeFrameHelper_win.h
LINUX_HEADERS =
MACOSX_HEADERS =


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

FORMS += \
	$$PWD/QtnRibbonRenameDialog.ui \
	$$PWD/QtnRibbonQuickAccessBarCustomizePage.ui \
	$$PWD/QtnRibbonBarCustomizePage.ui

RESOURCES += \
    $$PWD/QtnRibbonResources.qrc \
    $$PWD/QtnRibbonTranslations.qrc
   
win32 {
    DEFINES += _WINDOWS
    LIBS += -luser32 -lgdi32 -lshell32 -lcomctl32
}

macx {
    LIBS += -framework AppKit
}

QT += xml