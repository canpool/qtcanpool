VERSION = 2.1.1

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

DEFINES += QCUSTOMPLOT_COMPILE_LIBRARY

# remove some macros in qtproject.pri
DEFINES -= \
    QT_NO_JAVA_STYLE_ITERATORS \

HEADERS += \
    $$PWD/qcustomplot.h

SOURCES += \
    $$PWD/qcustomplot.cpp
