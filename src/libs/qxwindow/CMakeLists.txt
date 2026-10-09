# A browser has no native windowing system, and neither the Win32 nor the Cocoa
# backend can be compiled for WebAssembly at all, so the portable Qt backend is
# the only sensible default there. Mirrors the qmake build, which never picks a
# native backend for that target either.
if (QTCANPOOL_WASM)
  set(_qx_window_native_default NO)
else()
  set(_qx_window_native_default YES)
endif()

option(QX_WINDOW_CONFIG_NATIVE "Config native system window" ${_qx_window_native_default})
option(QX_WINDOW_CONFIG_SYSTEM_BORDERS "Config system borders" YES)
option(QX_WINDOW_CONFIG_STYLE_AGENT "Config style agent" NO)
option(QX_WINDOW_CONFIG_WIDGET_AGENT "Config widget agent" YES)

add_qtc_library(qxwindow
  VERSION 0.1.2
  DEFINES QX_WINDOW_LIBRARY
  PUBLIC_DEPENDS Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxwindow_global.h
    windowagentbase.cpp windowagentbase.h windowagentbase_p.h
    windowcontext.cpp windowcontext_p.h
    windowevent.cpp windowevent_p.h
    windowitemdelegate.cpp windowitemdelegate_p.h
    windowkit_p.h
    windowsystem_p.h
)

extend_qtc_library(qxwindow
  CONDITION QX_WINDOW_CONFIG_SYSTEM_BORDERS
  DEFINES QX_WINDOW_ENABLE_SYSTEM_BORDERS
)

if (QX_WINDOW_CONFIG_NATIVE)
  extend_qtc_library(qxwindow
    DEPENDS
      Qt5::CorePrivate Qt5::GuiPrivate
  )

  extend_qtc_library(qxwindow CONDITION WIN32
    SOURCES
      windowkit_win.cpp windowkit_win.h windowkit_win_p.h
      windowcontext_win.cpp
    DEPENDS
      user32 shell32 gdi32 advapi32
    DEFINES QX_WINDOW_NATIVE
  )

  extend_qtc_library(qxwindow CONDITION APPLE
    SOURCES
      windowcontext_cocoa.mm
    DEFINES QX_WINDOW_NATIVE
  )

  # The Qt fallback backend is Linux-only, exactly like the qmake build
  # (qxwindow-lib.pri adds windowcontext_qt.cpp under "linux*"): on macOS the
  # QX_WINDOW_NATIVE branch above declares WindowContextCocoa instead, and
  # compiling the Qt backend there would reference an undeclared class.
  extend_qtc_library(qxwindow CONDITION UNIX AND NOT APPLE
    SOURCES
      windowcontext_qt.cpp
      windowkit_linux.h
  )
else()
  extend_qtc_library(qxwindow
    SOURCES
      windowcontext_qt.cpp
  )
endif()


if (QX_WINDOW_CONFIG_WIDGET_AGENT)
  extend_qtc_library(qxwindow
    SOURCES
      windowagentwidget.cpp windowagentwidget.h windowagentwidget_p.h
      widgetitemdelegate.cpp widgetitemdelegate_p.h
  )

  if (QX_WINDOW_CONFIG_NATIVE)
    if (WIN32)
      extend_qtc_library(qxwindow
        SOURCES windowagentwidget_win.cpp
      )
    endif()

    if (APPLE)
      extend_qtc_library(qxwindow
        SOURCES windowagentwidget_mac.cpp
      )
    endif()
  endif()
endif()


if (QX_WINDOW_CONFIG_STYLE_AGENT)
  add_subdirectory(style)
endif()
