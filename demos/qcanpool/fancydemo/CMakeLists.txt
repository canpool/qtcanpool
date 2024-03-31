add_qtc_executable(fancydemo
  DEPENDS qcanpool Qt5::Core Qt5::Widgets
  SOURCES
    fancydemo.qrc
    fancydemo.rc
    main.cpp
    mainwindow.cpp mainwindow.h
)
