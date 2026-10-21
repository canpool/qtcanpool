add_qtc_library(qxfluent
  VERSION 0.1.0
  DEFINES QX_FLUENT_LIBRARY
  PUBLIC_DEPENDS qxcontrols Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxfluent_global.h
    fluenttokens.h fluenttokens.cpp
)
