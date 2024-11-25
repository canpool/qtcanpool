QTC_LIB_DEPENDS += qxdock
include(../../../qtproject.pri)

QT += widgets testlib

CONFIG += c++11
TARGET = tst_qxdock
DESTDIR = $$IDE_APP_PATH

include($$QTCANPOOL_DIR/src/rpath.pri)

include($$QTCANPOOL_DIR/tests/common/common.pri)

SOURCES += \
    $$files($$PWD/*.cpp)
