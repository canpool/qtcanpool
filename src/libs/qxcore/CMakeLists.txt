add_qtc_library(qxcore
  VERSION 0.1.1
  DEFINES QX_CORE_LIBRARY
  PUBLIC_DEPENDS Qt5::Core
  SOURCES
    qxcore_global.h
    qxlogger.cpp qxlogger.h
    qxsettings.cpp qxsettings.h
    qxtranslator.cpp qxtranslator.h
)
