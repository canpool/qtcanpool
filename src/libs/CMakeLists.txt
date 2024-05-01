option(WITH_QCANPOOL "Build qcanpool library" ON)
option(WITH_QXRIBBON "Build qxribbon library" ON)

if (WITH_QCANPOOL)
  add_subdirectory(qcanpool)
endif()

if (WITH_QXRIBBON)
  add_subdirectory(qxribbon)
endif()
