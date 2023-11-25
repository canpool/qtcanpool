# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0

BOOST_VERSION = 1_83
BOOST_ROOT = $$PWD/../../../3rdparty/Boost
BOOST_ROOT = $$absolute_path($$BOOST_ROOT)

!exists($$BOOST_ROOT) {
    error("$$BOOST_ROOT: No such directory")
}

# mingw
COMPILER_VERSION = mgw8-mt
BOOST_LIB_VERSION = x32
contains(QMAKE_HOST.arch, x86_64) {
    BOOST_LIB_VERSION = x64
}
BOOST_LIB_VERSION = $$BOOST_LIB_VERSION-$$BOOST_VERSION

BOOST_INC_PATH = $$BOOST_ROOT/include/boost-$$BOOST_VERSION
BOOST_LIB_PATH = $$BOOST_ROOT/lib

INCLUDEPATH += $$BOOST_INC_PATH
LIBS *= -L$$BOOST_LIB_PATH

defineReplace(boostLibraryTargetName) {
    unset(LIBRARY_NAME)
    LIBRARY_NAME = $$1
    RET = $$LIBRARY_NAME
    !isEmpty(COMPILER_VERSION):{
        RET = $$member(RET)-$$member(COMPILER_VERSION)
    }
    CONFIG(debug, debug|release) {
        !debug_and_release|build_pass {
            win32:RET = $$member(RET, 0)-d
        }
    }
    !isEmpty(BOOST_LIB_VERSION):{
        RET = $$member(RET)-$$member(BOOST_LIB_VERSION)
    }
    return($$RET)
}

defineReplace(boostLibraryName) {
   RET = $$boostLibraryTargetName($$1)
   return($$RET)
}
