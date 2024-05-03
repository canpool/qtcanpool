# Copyright (c) 2024 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0

QTITAN_VERSION = 1.5.1
QTITAN_ROOT = $$PWD/../../../3rdparty/qtitan-$$QTITAN_VERSION
QTITAN_ROOT = $$absolute_path($$QTITAN_ROOT)

!exists($$QTITAN_ROOT) {
    error("$$QTITAN_ROOT: No such directory")
}

# msvc 2019 on Qt 5.15.2
QTITAN_LIB_VERSION_LIST = $$split(QTITAN_VERSION, ".")
QTITAN_LIB_VERSION = $$member(QTITAN_LIB_VERSION_LIST, 0)

QTITAN_INC_PATH = $$QTITAN_ROOT/include
QTITAN_LIB_PATH = $$QTITAN_ROOT/lib

DEFINES += QTN_NAMESPACE

INCLUDEPATH += $$QTITAN_INC_PATH
LIBS *= -L$$QTITAN_LIB_PATH

defineReplace(QtitanLibraryTargetName) {
    unset(LIBRARY_NAME)
    LIBRARY_NAME = $$1
    RET = $$LIBRARY_NAME
    CONFIG(debug, debug|release) {
        !debug_and_release|build_pass {
            win32:RET = $$member(RET, 0)d
        }
    }
    !isEmpty(QTITAN_LIB_VERSION):{
        RET = $$member(RET)$$member(QTITAN_LIB_VERSION)
    }
    return($$RET)
}

defineReplace(QtitanLibraryName) {
   RET = $$QtitanLibraryTargetName($$1)
   return($$RET)
}
