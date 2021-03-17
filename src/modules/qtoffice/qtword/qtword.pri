
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/qthtmlword.h

SOURCES += \
    $$PWD/qthtmlword.cpp

win32 {
    greaterThan(QT_MAJOR_VERSION, 4){
        QT += axcontainer
    } else {
        CONFIG += qaxcontainer
    }

    HEADERS += \
        $$PWD/qtword.h

    SOURCES += \
        $$PWD/qtword.cpp
}
