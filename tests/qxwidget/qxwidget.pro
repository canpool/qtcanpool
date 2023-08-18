include(../../qtproject.pri)

QT += widgets testlib

CONFIG += c++11
TARGET = Test_qxwidget
DESTDIR = $$IDE_APP_PATH

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$QTCANPOOL_DIR/src/rpath.pri)

# lib
LIBS *= -l$$qtLibraryNameVersion(qxwidget, 0)

SOURCES += \
    main.cpp \
    tst_QxQuickAccessBar.cpp

HEADERS += \
    tst_QxQuickAccessBar.h
