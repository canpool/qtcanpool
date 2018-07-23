greaterThan(QT_MAJOR_VERSION, 4){
    QT += axcontainer
}else{
    CONFIG += qaxcontainer
}

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/qtexcel.h \
    $$PWD/qtxmlexcel.h

SOURCES += \
    $$PWD/qtexcel.cpp \
    $$PWD/qtxmlexcel.cpp
