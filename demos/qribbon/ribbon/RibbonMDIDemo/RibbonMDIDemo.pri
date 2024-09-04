greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += printsupport

HEADERS       = $$PWD/mainwindow.h \
                $$PWD/texteditchild.h \
                $$PWD/sketchchild.h \
                $$PWD/sketchdocument.h \
                $$PWD/sketchobject.h \
                $$PWD/sketchtool.h \
                $$PWD/sketchrepresentation.h \
                $$PWD/childinterface.h

SOURCES       = $$PWD/main.cpp \
                $$PWD/mainwindow.cpp \
                $$PWD/texteditchild.cpp \
                $$PWD/sketchchild.cpp \
                $$PWD/sketchdocument.cpp \
                $$PWD/sketchobject.cpp \
                $$PWD/sketchtool.cpp \
                $$PWD/sketchrepresentation.cpp

RESOURCES     = $$PWD/RibbonMDIDemo.qrc

macx {
    CONFIG-=app_bundle
}

