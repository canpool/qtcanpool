greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += printsupport

HEADERS        = $$PWD/mainwindow.h \
                 $$PWD/backstagepages.h

SOURCES        = $$PWD/main.cpp \
                 $$PWD/mainwindow.cpp \
                 $$PWD/backstagepages.cpp

FORMS         = $$PWD/infopage.ui \
                $$PWD/savepage.ui \
                $$PWD/helppage.ui \
                $$PWD/saveemailpage.ui \
                $$PWD/savewebpage.ui \

RESOURCES     = $$PWD/RibbonBackstageView.qrc

macx {
    CONFIG-=app_bundle
}
