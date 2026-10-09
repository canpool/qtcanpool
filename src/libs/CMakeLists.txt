option(WITH_QXCORE "Build qxcore library" ON)
option(WITH_QXTHEME "Build qxtheme library" ON)
option(WITH_QCANPOOL "Build qcanpool library" ON)
option(WITH_QXRIBBON "Build qxribbon library" ON)
option(WITH_QXDOCK   "Build qxdock library" ON)
option(WITH_QXWINDOW "Build qxwindow library" ON)
option(WITH_QXAPP "Build qxapp library" ON)

# qxcore is the infrastructure layer: it depends on Qt Core only, and every
# other library may build on top of it.
if (WITH_QXCORE)
  add_subdirectory(qxcore)
endif()

# qxtheme builds on qxcore (settings) and needs Qt Widgets (palette, style
# sheet). It is deliberately independent from qxribbon: the built-in style
# sheets are resolved from the qxribbon resource bundle at run time, so the
# theme engine stays usable in applications that do not use the ribbon.
if (WITH_QXTHEME AND WITH_QXCORE)
  add_subdirectory(qxtheme)
endif()

if (WITH_QCANPOOL)
  add_subdirectory(qcanpool)
endif()

if (WITH_QXRIBBON)
  add_subdirectory(qxribbon)
endif()

if (WITH_QXDOCK)
  add_subdirectory(qxdock)
endif()

if (WITH_QXWINDOW)
  add_subdirectory(qxwindow)
endif()

# qxapp is the application framework: it carries the ribbon window and the
# AppShell, which combines the navigation rail, the dock system, the theme
# engine and the settings, so it needs every layer underneath it.
if (WITH_QXAPP AND WITH_QXRIBBON AND WITH_QXWINDOW AND WITH_QXDOCK AND WITH_QXTHEME AND WITH_QXCORE)
  add_subdirectory(qxapp)
endif()
