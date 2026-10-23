add_qtc_library(qxcontrols
  VERSION 0.1.0
  DEFINES QX_CONTROLS_LIBRARY
  PUBLIC_DEPENDS qxtheme Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxcontrols_global.h
    qxcontrol.h qxcontrol.cpp
    qxcontrolaccessible.h qxcontrolaccessible.cpp
    qxstatelayer.h qxstatelayer.cpp
    qxpopupsurface.h qxpopupsurface.cpp
    qxringpainter.h qxringpainter.cpp
    qxshadowpainter.h qxshadowpainter.cpp
)
