greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += $$quote($$PWD)

HEADERS += \
    $$PWD/mainwindow.h \
    $$PWD/ribbonpopularpage.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/ribbonpopularpage.cpp

RESOURCES += \
    $$PWD/RibbonApplication.qrc

FORMS += \
    $$PWD/ribbonpopularpage.ui

macx {
    CONFIG-=app_bundle
}

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += printsupport
}
