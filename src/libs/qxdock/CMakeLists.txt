file(GLOB DOCK_SOURCES "*.cpp")
file(GLOB DOCK_HEADERS "*.h")
file(GLOB DOCK_QRCFILES "*.qrc")

list(REMOVE_ITEM DEFAULT_DEFINES
    QT_NO_JAVA_STYLE_ITERATORS
    QT_RESTRICTED_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
)

add_qtc_library(qxdock
  VERSION 0.2.0
  DEFINES QX_DOCK_LIBRARY
  PUBLIC_DEPENDS Qt5::Core Qt5::Widgets
  SOURCES
    ${DOCK_SOURCES}
    ${DOCK_HEADERS}
    ${DOCK_QRCFILES}
)

# Emscripten reports UNIX as well, but a browser has no X server and no xcb, so
# that target has to be excluded explicitly.
if (UNIX AND NOT APPLE AND NOT QTCANPOOL_WASM)
  # The X11 helpers in dockutils.h/.cpp use the Qt platform plugin private API
  # and link against xcb directly; DockFloatingTitleBar is the X11-only custom
  # title bar. Mirrors qxdock-lib.pri (QT += gui-private, LIBS += -lxcb).
  extend_qtc_library(qxdock
    SOURCES
      linux/dockfloatingtitlebar.cpp
      linux/dockfloatingtitlebar.h
    DEPENDS
      Qt5::GuiPrivate
      xcb
  )
endif()
