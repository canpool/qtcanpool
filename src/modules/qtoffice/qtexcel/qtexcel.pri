
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/qtxmlexcel.h

SOURCES += \
    $$PWD/qtxmlexcel.cpp

win32 {
    greaterThan(QT_MAJOR_VERSION, 4){
        QT += axcontainer
    } else {
        CONFIG += qaxcontainer
    }

    HEADERS += \
        $$PWD/qtexcel.h

    SOURCES += \
        $$PWD/qtexcel.cpp
}
