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
