option(WITH_QCANPOOL "Build qcanpool library" ON)
option(WITH_QXRIBBON "Build qxribbon library" ON)
option(WITH_QXDOCK   "Build qxdock library" ON)
option(WITH_QXWINDOW "Build qxwindow library" ON)

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
