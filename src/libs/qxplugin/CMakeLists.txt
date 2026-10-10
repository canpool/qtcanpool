add_qtc_library(qxplugin
  VERSION 0.1.0
  DEFINES QX_PLUGIN_LIBRARY
  PUBLIC_DEPENDS Qt5::Core Qt5::Widgets qxcore
  SOURCES
    qxplugin_global.h
    qxplugin.h
    qxplugin.cpp
    qxplugincontext.h
    qxplugincontext.cpp
    qxpluginspec.h
    qxpluginspec.cpp
    qxpluginmanager.h
    qxpluginmanager.cpp
)
