QTC_LIB_DEPENDS += qxribbon
include(../../qtproject.pri)

QT += widgets testlib

CONFIG += c++11
TARGET = tst_qxribbon
DESTDIR = $$IDE_APP_PATH

include($$QTCANPOOL_DIR/src/rpath.pri)

include($$QTCANPOOL_DIR/tests/common/common.pri)

SOURCES += \
    main.cpp \
    tst_ribbonquickaccessbar.cpp
