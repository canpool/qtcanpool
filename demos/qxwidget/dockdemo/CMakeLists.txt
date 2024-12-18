add_qtc_executable(DockDemo
  DEPENDS qxdock Qt5::Core Qt5::Widgets
  SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h
    res.qrc
)
