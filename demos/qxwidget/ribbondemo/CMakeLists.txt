add_qtc_executable(RibbonDemo
  DEPENDS qcanpool qxribbon Qt5::Core Qt5::Widgets
  SOURCES
    aboutdialog.cpp aboutdialog.h
    editablecontainer.cpp editablecontainer.h
    main.cpp
    mainwindow.cpp mainwindow.h
    ribbondemo.qrc
    ribbondemo.rc
)

extend_qtc_executable(RibbonDemo
  CONDITION WIN32
  DEFINES QXRIBBON_USE_APPWINDOW
  DEPENDS qxwindow
  SOURCES
    ribbonappwindow.cpp ribbonappwindow.h
)
