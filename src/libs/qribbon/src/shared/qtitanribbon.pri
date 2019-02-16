
QTITAN_ROOT = $$PWD/../..

include($$QTITAN_ROOT/qtitan.pri)
include($$QTITAN_ROOT/src/shared/platform.pri)

QTITAN_LIB_PATH = $$QTITAN_ROOT/lib$$QTITAN_LIB_PREFIX
contains(QMAKE_HOST.arch, x86_64) {
    QTITAN_LIB_PATH = $$QTITAN_LIB_PATH/X64
}
INCLUDEPATH += $$QTITAN_ROOT/include

win32 {
        win32-g++ {
                LIBS += -L$$QTITAN_LIB_PATH
                CONFIG(debug, debug|release):LIBS += -lqtnribbond3
                        else:LIBS += -lqtnribbon3
        }
        else {
                CONFIG(debug, debug|release):LIBS += $$QTITAN_LIB_PATH/qtnribbond3.lib
                        else:LIBS += $$QTITAN_LIB_PATH/qtnribbon3.lib
        }
}

unix {

    LIBS += -L$$QTITAN_LIB_PATH

    CONFIG(debug, debug|release):LIBS += -lqtnribbond
        else:LIBS += -lqtnribbon
}

# DLL
isEmpty(QTITAN_DLL_FILE): {
    win32 {
        CONFIG(debug, debug|release):QTITAN_DLL_FILE = qtnribbond3.dll
                else:QTITAN_DLL_FILE = qtnribbon3.dll
    }

    unix {
        CONFIG(debug, debug|release):QTITAN_DLL_FILE = qtnribbond.so
            else:QTITAN_DLL_FILE = qtnribbon.so
    }
}

files = $$files($$QTITAN_LIB_PATH/$$member(QTITAN_DLL_FILE), false)
#for(file, files):contains(file, ".*\\.dll$"):!exists($$file/*): \
for(file, files):!exists($$file/*): \
    STATIC_FILES += $$file

STATIC_BASE = $$QTITAN_LIB_PATH
STATIC_INSTALL_BASE = $$INSTALL_DATA_PATH

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets
}
