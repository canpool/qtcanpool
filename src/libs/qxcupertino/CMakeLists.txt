add_qtc_library(qxcupertino
  VERSION 0.1.0
  DEFINES QX_CUPERTINO_LIBRARY
  PUBLIC_DEPENDS qxcontrols Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxcupertino_global.h
    cupertinotokens.h cupertinotokens.cpp
    cupertinoswitch.h cupertinoswitch.cpp
    cupertinoslider.h cupertinoslider.cpp
)
