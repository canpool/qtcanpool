HEADERS       = $$PWD/mainwindow.h \
                $$PWD/ribbonpopularpage.h

SOURCES       = $$PWD/main.cpp \
                $$PWD/mainwindow.cpp \
                $$PWD/ribbonpopularpage.cpp

RESOURCES     = $$PWD/Application.qrc
FORMS         = $$PWD/ribbonpopularpage.ui \
                $$PWD/ribbonsample.ui

include($$PWD/../../shared/aboutdialog.pri)
include($$PWD/../../ribbon/shared/ribbonwindow.pri)
