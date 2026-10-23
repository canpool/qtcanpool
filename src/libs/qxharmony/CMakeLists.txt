add_qtc_library(qxharmony
  VERSION 0.1.0
  DEFINES QX_HARMONY_LIBRARY
  PUBLIC_DEPENDS qxcontrols Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxharmony_global.h
    harmonytokens.h harmonytokens.cpp
    harmonyswitch.h harmonyswitch.cpp
)
