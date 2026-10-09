option(WITH_QXCORE "Build qxcore library" ON)
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

if (WITH_QXAPP AND WITH_QXRIBBON AND WITH_QXWINDOW)
  add_subdirectory(qxapp)
endif()
