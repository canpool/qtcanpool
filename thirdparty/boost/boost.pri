
BOOST_ROOT = C:/Boost

INCLUDEPATH = $$BOOST_ROOT/include/boost-1_67
BOOST_LIB_PATH = $$BOOST_ROOT/lib

LIBS *= -L$$BOOST_LIB_PATH

COMPILER_VERSION = mgw53-mt
BOOST_VERSION = x32-1_67

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
    !isEmpty(BOOST_VERSION):{
        RET = $$member(RET)-$$member(BOOST_VERSION)
    }
    return($$RET)
}

defineReplace(boostLibraryName) {
   RET = $$boostLibraryTargetName($$1)
   return($$RET)
}
