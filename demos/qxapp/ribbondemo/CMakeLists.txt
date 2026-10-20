set(_ribbondemo_depends qxribbon qxtheme Qt5::Core Qt5::Widgets)
set(_ribbondemo_defines)

# Same choice as ribbondemo.pro used to make: on Windows with Qt 6 the frameless
# window comes from qxapp.
#
# NOTE: the test is written against Qt5_VERSION, not QT_VERSION_MAJOR. Qt6 publishes
# QT_VERSION_MAJOR as a *directory property* (Qt6Config.cmake) rather than a variable,
# so ${QT_VERSION_MAJOR} expands to an empty string and "" GREATER 5 is false - the
# condition silently dropped qxapp and this demo kept running on its own frameless
# window even on Qt 6. This is also how the rest of the tree asks the question
# (see CMakeLists.txt).
#
# Since 4.3 the other side of this switch is the plain RibbonMainWindow: qxribbon no
# longer carries a frameless window of its own, so "no qxapp" now means "no frameless"
# rather than "the incomplete frameless path".
if (WIN32 AND NOT Qt5_VERSION VERSION_LESS 6.0.0)
  list(APPEND _ribbondemo_depends qxapp)
  list(APPEND _ribbondemo_defines QXRIBBON_USE_APPWINDOW)
endif()

# QxTranslator is the qxcore class the demo loads the library catalogues through.
set(_ribbondemo_depends ${_ribbondemo_depends} qxcore)

# Where the built catalogues are. The paths are defines rather than something
# walked at run time because they are decided by the install layout - and the
# build tree repeats that layout under CMAKE_BINARY_DIR, so what the demo is
# told here is equally true of a build tree and of something that was installed.
# Absent when lrelease was missing, in which case there is nothing to load and
# --lang has no effect rather than pointing at a directory that does not exist.
if (TARGET Qt5::lrelease)
  list(APPEND _ribbondemo_defines
    "QTCANPOOL_TRANSLATIONS_DIR=\"${CMAKE_BINARY_DIR}/${IDE_DATA_PATH}/translations\"")
endif()

add_qtc_executable(RibbonDemo
  DEPENDS ${_ribbondemo_depends}
  DEFINES ${_ribbondemo_defines}
  SOURCES
    aboutdialog.cpp aboutdialog.h
    applypage.cpp applypage.h
    contextpage1.cpp contextpage1.h
    contextpage2.cpp contextpage2.h
    custompage.cpp custompage.h
    homepage.cpp homepage.h
    otherpage.cpp otherpage.h
    main.cpp
    mainwindow.cpp mainwindow.h
    ribbondemo.qrc
    ribbondemo.rc
    # The style sheets the demo offers as themes (:/themes/*.css).
    ${CMAKE_SOURCE_DIR}/res/themes.qrc
)

if (TARGET generate_qm_files)
  add_dependencies(RibbonDemo generate_qm_files)
endif()
