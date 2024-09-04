greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += printsupport

HEADERS        = $$PWD/mainwindow.h \
                 $$PWD/galleryItems.h

SOURCES        = $$PWD/main.cpp \
                 $$PWD/mainwindow.cpp \
                 $$PWD/galleryItems.cpp

RESOURCES     = $$PWD/RibbonGalleriesDemo.qrc

macx {
    CONFIG-=app_bundle
}
