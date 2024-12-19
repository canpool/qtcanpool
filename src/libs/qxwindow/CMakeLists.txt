option(QX_WINDOW_CONFIG_NATIVE "Config native system window" YES)
option(QX_WINDOW_CONFIG_SYSTEM_BORDERS "Config system borders" YES)
option(QX_WINDOW_CONFIG_STYLE_AGENT "Config style agent" NO)
option(QX_WINDOW_CONFIG_WIDGET_AGENT "Config widget agent" YES)

add_qtc_library(qxwindow
  VERSION 0.1.1
  DEFINES QX_WINDOW_LIBRARY
  DEPENDS Qt5::Core Qt5::Gui Qt5::Widgets
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
  )

  extend_qtc_library(qxwindow CONDITION UNIX
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
