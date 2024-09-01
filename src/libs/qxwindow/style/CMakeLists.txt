list(APPEND STYLE_SOURCES windowstyleagent.cpp windowstyleagent.h windowstyleagent_p.h)

if (APPLE)
  list(APPEND STYLE_SOURCES windowstyleagent_mac.cpp)
endif()

if (UNIX)
  list(APPEND STYLE_SOURCES windowstyleagent_linux.cpp)
endif()

if (WIN32)
  list(APPEND STYLE_SOURCES windowstyleagent_win.cpp)
endif()

extend_qtc_library(qxwindow
  SOURCES_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}
  DEFINES QX_WINDOW_ENABLE_STYLE_AGENT
  SOURCES ${STYLE_SOURCES}
)
