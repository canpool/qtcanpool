greaterThan(QT_MAJOR_VERSION, 4){
    QT += axcontainer
}else{
    CONFIG += qaxcontainer
}

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/qtword.h \
    $$PWD/qthtmlword.h

SOURCES += \
    $$PWD/qtword.cpp \
    $$PWD/qthtmlword.cpp
