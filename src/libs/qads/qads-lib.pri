VERSION = 4.0.2

SRC_DIR = $$PWD/Qt-Advanced-Docking-System/src
INCLUDEPATH += $$SRC_DIR $$SRC_DIR/linux
DEFINES -= QT_NO_JAVA_STYLE_ITERATORS QT_RESTRICTED_CAST_FROM_ASCII

CONFIG += c++14
DEFINES += QT_DEPRECATED_WARNINGS
QT += core gui widgets

!adsBuildStatic {
    CONFIG += shared
    DEFINES += ADS_SHARED_EXPORT
}
adsBuildStatic {
    CONFIG += staticlib
    DEFINES += ADS_STATIC
}

windows {
	# MinGW
	*-g++* {
		QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
	}
	# MSVC
	*-msvc* {
                QMAKE_CXXFLAGS += /utf-8
        }
}

RESOURCES += $$SRC_DIR/ads.qrc

HEADERS += \
    $$SRC_DIR/ads_globals.h \
    $$SRC_DIR/DockAreaWidget.h \
    $$SRC_DIR/DockAreaTabBar.h \
    $$SRC_DIR/DockContainerWidget.h \
    $$SRC_DIR/DockManager.h \
    $$SRC_DIR/DockWidget.h \
    $$SRC_DIR/DockWidgetTab.h \
    $$SRC_DIR/DockingStateReader.h \
    $$SRC_DIR/FloatingDockContainer.h \
    $$SRC_DIR/FloatingDragPreview.h \
    $$SRC_DIR/DockOverlay.h \
    $$SRC_DIR/DockSplitter.h \
    $$SRC_DIR/DockAreaTitleBar_p.h \
    $$SRC_DIR/DockAreaTitleBar.h \
    $$SRC_DIR/ElidingLabel.h \
    $$SRC_DIR/IconProvider.h \
    $$SRC_DIR/DockComponentsFactory.h  \
    $$SRC_DIR/DockFocusController.h \
    $$SRC_DIR/AutoHideDockContainer.h \
    $$SRC_DIR/AutoHideSideBar.h \
    $$SRC_DIR/AutoHideTab.h \
    $$SRC_DIR/PushButton.h \
    $$SRC_DIR/ResizeHandle.h


SOURCES += \
    $$SRC_DIR/ads_globals.cpp \
    $$SRC_DIR/DockAreaWidget.cpp \
    $$SRC_DIR/DockAreaTabBar.cpp \
    $$SRC_DIR/DockContainerWidget.cpp \
    $$SRC_DIR/DockManager.cpp \
    $$SRC_DIR/DockWidget.cpp \
    $$SRC_DIR/DockingStateReader.cpp \
    $$SRC_DIR/DockWidgetTab.cpp \
    $$SRC_DIR/FloatingDockContainer.cpp \
    $$SRC_DIR/FloatingDragPreview.cpp \
    $$SRC_DIR/DockOverlay.cpp \
    $$SRC_DIR/DockSplitter.cpp \
    $$SRC_DIR/DockAreaTitleBar.cpp \
    $$SRC_DIR/ElidingLabel.cpp \
    $$SRC_DIR/IconProvider.cpp \
    $$SRC_DIR/DockComponentsFactory.cpp \
    $$SRC_DIR/DockFocusController.cpp \
    $$SRC_DIR/AutoHideDockContainer.cpp \
    $$SRC_DIR/AutoHideSideBar.cpp \
    $$SRC_DIR/AutoHideTab.cpp \
    $$SRC_DIR/PushButton.cpp \
    $$SRC_DIR/ResizeHandle.cpp


unix:!macx {
HEADERS += $$SRC_DIR/linux/FloatingWidgetTitleBar.h
SOURCES += $$SRC_DIR/linux/FloatingWidgetTitleBar.cpp
LIBS += -lxcb
QT += gui-private
}
