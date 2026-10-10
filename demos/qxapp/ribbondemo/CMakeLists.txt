set(_ribbondemo_depends qxribbon Qt5::Core Qt5::Widgets)
set(_ribbondemo_defines)

# 对齐 ribbondemo.pro：Qt6 + Windows 下使用 qxapp 提供无边框窗口
if (WIN32 AND QT_VERSION_MAJOR GREATER 5)
  list(APPEND _ribbondemo_depends qxapp)
  list(APPEND _ribbondemo_defines QXRIBBON_USE_APPWINDOW)
endif()

add_qtc_executable(RibbonDemo
  DEPENDS ${_ribbondemo_depends}
  DEFINES ${_ribbondemo_defines}
  SOURCES
    aboutdialog.cpp aboutdialog.h
    main.cpp
    mainwindow.cpp mainwindow.h
    ribbondemo.qrc
    ribbondemo.rc
)
