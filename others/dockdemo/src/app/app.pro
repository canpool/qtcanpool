QTC_LIB_DEPENDS += qads
include(../../qtproject.pri)

QT       += core gui widgets quick quickwidgets

lessThan(QT_MAJOR_VERSION, 6) {
    win32 {
        QT += axcontainer
    }
}

TARGET = DockDemo
TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

# remove some macros in qtproject.pri
DEFINES -= \
    QT_NO_JAVA_STYLE_ITERATORS \
    QT_NO_CAST_TO_ASCII \
    QT_RESTRICTED_CAST_FROM_ASCII \
    "QT_DISABLE_DEPRECATED_BEFORE=0x050900"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

DEMO_DIR = $$QTCANPOOL_DIR/src/libs/qads/Qt-Advanced-Docking-System/demo

!exists($$DEMO_DIR) {
    error("The Qt-Advanced-Docking-System code is not prepared")
}

INCLUDEPATH += $$DEMO_DIR

RC_FILE += $$DEMO_DIR/app.rc

adsBuildStatic {
    DEFINES += ADS_STATIC
}

HEADERS += \
	$$DEMO_DIR/MainWindow.h \
	$$DEMO_DIR/StatusDialog.h \
	$$DEMO_DIR/ImageViewer.h \
	$$DEMO_DIR/RenderWidget.h

SOURCES += \
	$$DEMO_DIR/main.cpp \
	$$DEMO_DIR/MainWindow.cpp \
	$$DEMO_DIR/StatusDialog.cpp \
	$$DEMO_DIR/ImageViewer.cpp \
	$$DEMO_DIR/RenderWidget.cpp

FORMS += \
	$$DEMO_DIR/mainwindow.ui \
	$$DEMO_DIR/StatusDialog.ui

RESOURCES += $$DEMO_DIR/demo.qrc
