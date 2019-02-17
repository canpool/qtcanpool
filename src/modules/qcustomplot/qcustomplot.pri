greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/qcustomplot.h \
    $$PWD/qcustomchart.h

SOURCES += \
    $$PWD/qcustomplot.cpp \
    $$PWD/qcustomchart.cpp

RESOURCES += \
    $$PWD/chart.qrc


