add_qtc_executable(DockDemo
  DEPENDS qxdock Qt5::Core Qt5::Widgets
  SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h
    res.qrc
    # The dock style sheet this demo installs in mainwindow.cpp; the library
    # stopped shipping one. res/dockstyle.qrc is shared with any other application
    # that wants the same dock look, the way res/themes.qrc is for the ribbon.
    ${CMAKE_SOURCE_DIR}/res/dockstyle.qrc
)
