set(_ribbondemo_depends qxribbon Qt5::Core Qt5::Widgets)
set(_ribbondemo_defines)

# Same choice as ribbondemo.pro used to make: on Windows with Qt 6 the frameless
# window comes from qxapp.
#
# NOTE: the test is written against Qt5_VERSION, not QT_VERSION_MAJOR. Qt6 publishes
# QT_VERSION_MAJOR as a *directory property* (Qt6Config.cmake) rather than a variable,
# so ${QT_VERSION_MAJOR} expands to an empty string and "" GREATER 5 is false - the
# condition silently dropped qxapp and this demo kept running on RibbonWindow, the
# frameless helper, even on Qt 6. This is also how the rest of the tree asks the
# question (see CMakeLists.txt and src/libs/qxribbon/CMakeLists.txt).
if (WIN32 AND NOT Qt5_VERSION VERSION_LESS 6.0.0)
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
