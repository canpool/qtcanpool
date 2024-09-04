greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += printsupport

HEADERS        = $$PWD/mainwindow.h

SOURCES        = $$PWD/main.cpp \
                 $$PWD/mainwindow.cpp

RESOURCES     = $$PWD/RibbonControls.qrc

macx {
    CONFIG-=app_bundle
}
