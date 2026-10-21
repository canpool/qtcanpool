add_qtc_library(qxmaterial
  VERSION 0.1.0
  DEFINES QX_MATERIAL_LIBRARY
  PUBLIC_DEPENDS qxcontrols Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxmaterial_global.h
    materialtokens.h materialtokens.cpp
)
