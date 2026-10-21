# qtcanpool tests (CMake / CTest)
#
# The one and only entry point of the test build, active with -DWITH_TESTS=ON
# (the qmake project file went away in 4.0).
#
# It reuses add_qtc_test() from the framework, which registers the test with
# CTest and takes care of rpath / automoc / include paths (the public include
# directory of every library is src/libs, so a test can simply
# #include "qxribbon/xxx.h" or "qxdock/xxx.h").
#
# The per-suite source lists are globs so that adding tst_foo.cpp is all it
# takes to add a test. CONFIGURE_DEPENDS is what makes that true: without it the
# new file is not in the build system until someone re-runs the configure step
# by hand, and the error that follows (an undefined symbol for the test class,
# or a missing .moc) does not mention the static file list at all.

set(QTCANPOOL_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# ------------------------------------------------------------------- qxcore --
# The translator test proves that switching language really changes what
# translate() returns, and proving that needs a compiled catalogue: QTranslator
# parses the file it loads, so a stand-in with the right name will not do.
# lrelease ships with LinguistTools and stays optional - without it the test
# skips that layer rather than pretending it passed.
set(QXCORE_TRANSLATION_DIR "${CMAKE_CURRENT_BINARY_DIR}/translations")
set(QXCORE_TRANSLATION_DEFINES "")
if (TARGET Qt5::lrelease)
  file(MAKE_DIRECTORY "${QXCORE_TRANSLATION_DIR}")
  set(_qxcore_qm_files "")
  foreach(_language fr zh_CN)
    set(_ts "${QTCANPOOL_TESTS_DIR}/qxcore/auto/data/qxtranslator_${_language}.ts")
    set(_qm "${QXCORE_TRANSLATION_DIR}/qxtranslator_${_language}.qm")
    add_custom_command(OUTPUT "${_qm}"
        COMMAND Qt5::lrelease "${_ts}" -qm "${_qm}"
        MAIN_DEPENDENCY "${_ts}"
        COMMENT "Compile the ${_language} test translation"
        VERBATIM
    )
    list(APPEND _qxcore_qm_files "${_qm}")
  endforeach()
  add_custom_target(qxcore_test_translations DEPENDS ${_qxcore_qm_files})
  set(QXCORE_TRANSLATION_DEFINES "QXCORE_TEST_TRANSLATIONS=\"${QXCORE_TRANSLATION_DIR}\"")
else()
  message(STATUS "lrelease not found: the translator test will skip its load layer")
endif()

# Three libraries ship strings and therefore ship a catalogue for them, and each
# one is checked by reading it back from both ends: the .ts says what every
# string promised, the compiled .qm says what Qt really answers when asked
# through the very context the .ts recorded. A catalogue that quietly stops
# matching its own source is what that catches. Both reach the test as defines
# because this file is the only place that knows where the build put things.
set(QTCANPOOL_CATALOGUE_DEFINES "")
if (TARGET Qt5::lrelease)
  set(QTCANPOOL_CATALOGUE_DEFINES
    "QTCANPOOL_TRANSLATIONS_DIR=\"${CMAKE_BINARY_DIR}/${IDE_DATA_PATH}/translations\""
    "QTCANPOOL_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\""
  )
endif()

file(GLOB QXCORE_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxcore/auto/tst_*.cpp")
add_qtc_test(tst_qxcore
    DEPENDS qxcore
    DEFINES ${QXCORE_TRANSLATION_DEFINES}
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxcore/auto/main.cpp"
        ${QXCORE_TEST_SOURCES}
)
if (TARGET qxcore_test_translations)
  add_dependencies(tst_qxcore qxcore_test_translations)
endif()

# ---------------------------------------------------------------- qxtheme --
file(GLOB QXTHEME_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxtheme/auto/tst_*.cpp")
add_qtc_test(tst_qxtheme
    DEPENDS qxtheme
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxtheme/auto/main.cpp"
        ${QXTHEME_TEST_SOURCES}
)

# ------------------------------------------------------------ qxcontrols --
file(GLOB QXCONTROLS_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxcontrols/auto/tst_*.cpp")
add_qtc_test(tst_qxcontrols
    DEPENDS qxcontrols
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxcontrols/auto/main.cpp"
        ${QXCONTROLS_TEST_SOURCES}
)

# -------------------------------------------------------------- qxfluent --
file(GLOB QXFLUENT_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxfluent/auto/tst_*.cpp")
add_qtc_test(tst_qxfluent
    DEPENDS qxfluent
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxfluent/auto/main.cpp"
        ${QXFLUENT_TEST_SOURCES}
)

# ------------------------------------------------------------- qxmaterial --
file(GLOB QXMATERIAL_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxmaterial/auto/tst_*.cpp")
add_qtc_test(tst_qxmaterial
    DEPENDS qxmaterial
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxmaterial/auto/main.cpp"
        ${QXMATERIAL_TEST_SOURCES}
)

# ------------------------------------------------------------------- qxapp --
# qxapp is the application framework, so its test links the whole stack: the
# shell builds a ribbon window with a dock area and a theme.
file(GLOB QXAPP_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxapp/auto/tst_*.cpp")
add_qtc_test(tst_qxapp
    DEPENDS qxapp
    DEFINES ${QTCANPOOL_CATALOGUE_DEFINES}
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_catalogue.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_catalogue.h"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxapp/auto/main.cpp"
        ${QXAPP_TEST_SOURCES}
        # The bundled themes, so registerBundledThemes() has something to read
        # and the roles that travel beside each sheet can be checked.
        ${CMAKE_SOURCE_DIR}/res/themes.qrc
        # The dock's fragments, so applicationStyleSheet() has something to
        # append to a theme's sheet - without them the dock's half is empty and
        # an assertion about its colours passes for the wrong reason.
        ${CMAKE_SOURCE_DIR}/res/dockstyle.qrc
)
if (TARGET generate_qm_files)
  add_dependencies(tst_qxapp generate_qm_files)
endif()

# ----------------------------------------------------- qxapp examples --
# In-process smoke tests for the four family examples added in 4.4.1. Each test
# compiles the example's own MainWindow source and stands the window up
# offscreen, so an example cannot rot silently: if its source stops building or
# its window starts crashing, the test fails.
#
# Launching the example executables through QProcess was tried first and is not
# viable here - CreateProcess fails for them on Windows no matter what
# environment, working directory or argument list is used - so the example is
# exercised in-process instead, which also keeps the test independent of how the
# example is deployed.
#
# The example targets only exist when examples are built, hence the guard.
set(QTCANPOOL_EXAMPLES_DIR "${CMAKE_SOURCE_DIR}/examples")
macro(add_qxapp_example_test dir target)
  if (TARGET ${target})
    add_qtc_test(tst_qxappexample_${dir}
        DEPENDS qxapp
        INCLUDES
            "${QTCANPOOL_TESTS_DIR}/common"
            "${QTCANPOOL_EXAMPLES_DIR}/qxapp/${dir}"
        SOURCES
            "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
            "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
            "${QTCANPOOL_TESTS_DIR}/qxapp/auto/main.cpp"
            "${QTCANPOOL_EXAMPLES_DIR}/qxapp/${dir}/mainwindow.cpp"
            "${QTCANPOOL_TESTS_DIR}/qxapp/examples/tst_qxappexample_${dir}.cpp"
    )
    add_dependencies(tst_qxappexample_${dir} ${target})
  endif()
endmacro()
add_qxapp_example_test(propertyeditor Example_App_PropertyEditor)
add_qxapp_example_test(buttons Example_App_Buttons)
add_qxapp_example_test(navigationbar Example_App_NavigationBar)
add_qxapp_example_test(tabwidget Example_App_TabWidget)

# --------------------------------------------------------------- qxplugin --
# The plugin runtime: contract, discovery, dependency resolution and failure
# isolation. The host-adapter case links qxapp to drive a real QxAppShell as
# the QxPluginContext, proving plugins never touch the window class.
file(GLOB QXPLUGIN_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxplugin/auto/tst_*.cpp")
add_qtc_test(tst_qxplugin
    DEPENDS qxapp qxplugin
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxplugin/auto/main.cpp"
        ${QXPLUGIN_TEST_SOURCES}
)

# -------------------------------------------------------------- qxmodules --
# The module samples are plugins, so this suite loads them the way D1's shell
# will: out of the directory they were built into, through QxPluginManager. The
# directory is baked in as a definition rather than guessed, because the
# framework's plugin layout is per platform.
#
# Guarded by the target: with -DWITH_MODULES=OFF there is nothing to load.
if (TARGET output)
  file(GLOB QXMODULES_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxmodules/auto/tst_*.cpp")
  add_qtc_test(tst_qxmodules
      DEPENDS qxapp qxplugin qxdock
      DEFINES "QXMODULES_PLUGIN_DIR=\"$<TARGET_FILE_DIR:output>\""
      INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
      SOURCES
          "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
          "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
          "${QTCANPOOL_TESTS_DIR}/qxmodules/auto/main.cpp"
          ${QXMODULES_TEST_SOURCES}
  )
  # The test loads the modules, so they have to exist before it runs.
  add_dependencies(tst_qxmodules output notebook filetree)
endif()

# ---------------------------------------------------------------- qxribbon --
file(GLOB QXRIBBON_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxribbon/auto/tst_*.cpp")
add_qtc_test(tst_qxribbon
    DEPENDS qxribbon
    DEFINES ${QTCANPOOL_CATALOGUE_DEFINES}
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_catalogue.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_catalogue.h"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/common/tst_probe.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_probe.h"
        "${QTCANPOOL_TESTS_DIR}/qxribbon/auto/main.cpp"
        ${QXRIBBON_TEST_SOURCES}
)
if (TARGET generate_qm_files)
  # The catalogue has to be compiled before the suite that reads it runs, and
  # "ALL" alone does not say it happens first.
  add_dependencies(tst_qxribbon generate_qm_files)
endif()

# ------------------------------------------------------------------ qxdock --
file(GLOB QXDOCK_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxdock/auto/tst_*.cpp")
add_qtc_test(tst_qxdock
    # qxtheme is here for tst_DockTheme alone: a dock answers a theme switch
    # through the sheet the application installs, and a theme is the only honest
    # way to change that sheet. The dock itself knows nothing about themes - it
    # reads qApp - so the dependency belongs to the test and not to the library.
    DEPENDS qxdock qxtheme
    DEFINES ${QTCANPOOL_CATALOGUE_DEFINES}
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        # The dock style sheet is the application's to build in, so this suite has
        # to be one: tst_DockWindow::styleSheetIsReadFromTheApplication reads it
        # through the public entry point, and without the resource there would be
        # nothing to read and a DockWindow would look unstyled without anything
        # failing. In SOURCES rather than a keyword of its own because
        # add_qtc_test() takes none - AUTORCC picks the .qrc up from here.
        "${PROJECT_SOURCE_DIR}/res/dockstyle.qrc"
        "${QTCANPOOL_TESTS_DIR}/common/tst_catalogue.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_catalogue.h"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/common/tst_probe.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_probe.h"
        "${QTCANPOOL_TESTS_DIR}/qxdock/auto/main.cpp"
        ${QXDOCK_TEST_SOURCES}
)
if (TARGET generate_qm_files)
  add_dependencies(tst_qxdock generate_qm_files)
endif()

# -------------------------------------------------------------- qxwindow --
# qxwindow is platform-heavy, so this suite sticks to the backend-independent
# bookkeeping the shared WindowContext does. See the test file for the reasoning.
#
# The CorePrivate/GuiPrivate dependencies are for tst_QxWindowWinKit, which
# includes windowkit_win_p.h under Q_OS_WIN. extend_qtc_target puts DEPENDS under
# PRIVATE, so depending on qxwindow does not inherit the Qt private modules
# qxwindow itself uses, and the include path for qsystemlibrary_p.h is missing
# without these. Only the Windows leg compiles that file.
file(GLOB QXWINDOW_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxwindow/auto/tst_*.cpp")
add_qtc_test(tst_qxwindow
    DEPENDS qxwindow Qt5::CorePrivate Qt5::GuiPrivate
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxwindow/auto/main.cpp"
        ${QXWINDOW_TEST_SOURCES}
)

# ------------------------------------------------------- qxwindow (native) --
# The Windows window procedure, reached through a real window. This is a target
# of its own rather than a file in tst_qxwindow above, because it has to run
# under QT_QPA_PLATFORM=windows while every other suite has to stay on offscreen.
#
# With the platform it does not want, every case skips, so an ordinary offscreen
# pass reports it as a no-op instead of a failure - and the CI job that runs it
# natively (a non-gating step, see ci.yml) is what actually exercises it. Only
# WIN32: there is no window procedure anywhere else.
#
# QX_WINDOW_ENABLE_SYSTEM_BORDERS is passed on rather than re-derived: it tells
# the cases which of the two frame shapes to expect, and the only authority on
# that is the option the library itself was built with (see
# src/libs/qxwindow/CMakeLists.txt). Guessing it from the Windows version instead
# would let the two drift apart silently.
if (WIN32)
  set(QXWINDOW_NATIVE_TEST_DEFINES)
  if (QX_WINDOW_CONFIG_SYSTEM_BORDERS)
    list(APPEND QXWINDOW_NATIVE_TEST_DEFINES QX_WINDOW_ENABLE_SYSTEM_BORDERS)
  endif()

  file(GLOB QXWINDOW_NATIVE_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxwindow/native/tst_*.cpp")
  add_qtc_test(tst_qxwindow_native
      DEPENDS qxwindow
      DEFINES ${QXWINDOW_NATIVE_TEST_DEFINES}
      INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
      SOURCES
          "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
          "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
          "${QTCANPOOL_TESTS_DIR}/qxwindow/native/main.cpp"
          ${QXWINDOW_NATIVE_TEST_SOURCES}
  )
  set_tests_properties(tst_qxwindow_native PROPERTIES
      LABELS native
      TIMEOUT 120
  )
endif()

# ------------------------------------------------------------- deprecated --
# The QX_<LIB>_DEPRECATED_SINCE gate every library carries in its *_global.h. It
# is header-only, so this suite links nothing but QtCore and reads all seven
# headers; tst_qxdeprecated_cutoff.cpp sets a cutoff before including one of them,
# which is why the opt-out case needs a translation unit of its own.
file(GLOB QXDEPRECATED_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/deprecated/auto/tst_*.cpp")
add_qtc_test(tst_qxdeprecated
    DEPENDS Qt5::Core
    # src/libs is the public include directory of every library, and it normally
    # arrives through DEPENDS. This suite reads headers only, so it asks for the
    # directory itself instead of linking a library it never calls into.
    INCLUDES "${PROJECT_SOURCE_DIR}/src/libs" "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/deprecated/auto/main.cpp"
        ${QXDEPRECATED_TEST_SOURCES}
)

# -------------------------------------------------------------- helloworld --
# Minimal tests without a library dependency (QTEST_MAIN provides the entry point).
add_qtc_test(tst_qstring
    DEPENDS Qt5::Core
    SOURCES "${QTCANPOOL_TESTS_DIR}/helloworld/qstring/tst_qstring.cpp"
)
add_qtc_test(tst_qlineedit
    DEPENDS Qt5::Widgets
    SOURCES "${QTCANPOOL_TESTS_DIR}/helloworld/qlineedit/tst_qlineedit.cpp"
)

# --------------------------------------------------------------- ideshell --
# The IDE-style sample application (plan 4.0, task D1) and the carrier of
# acceptance criterion #15. The demo is run rather than linked - its whole claim
# is that an application can be assembled from modules it does not know, and a
# link here would compile that claim away. It needs the modules on disk, hence
# the dependencies; with -DWITH_DEMOS=OFF there is no demo and no test.
if (TARGET IdeShellDemo)
  add_qtc_test(tst_ideshelldemo
      DEPENDS Qt5::Core
      DEFINES
          DEMO_IDESHELL_EXECUTABLE="$<TARGET_FILE:IdeShellDemo>"
      SOURCES "${QTCANPOOL_TESTS_DIR}/ideshell/auto/tst_ideshelldemo.cpp"
  )
  add_dependencies(tst_ideshelldemo IdeShellDemo output notebook filetree)
endif()

# ------------------------------------------------------- demo screenshots --
# Visual regression for the demos (roadmap task B4).
#
# The demos render themselves offscreen through their own --screenshot switch,
# which keeps the window driving out of the test, and each capture is compared
# with a golden image. The golden is per platform *and* per Qt major version
# (tests/demo/golden/<CMAKE_SYSTEM_NAME>/Qt<N>/), because a screenshot is the one
# artefact here that legitimately differs between machines - same code, a
# different font rasteriser, different pixels - and the
# comparison is opt-in (QTCANPOOL_VISUAL_BASELINE=1) because it is only
# meaningful between the machine that produced the golden and the same machine
# later; CI, with its own fonts, must never judge one.
#
# Registered with the label "visual" so it can be filtered out of a run.
# NOTE: no ENVIRONMENT here - add_qtc_test() bakes the configure-time PATH
# (MinGW runtime + Qt DLLs) into that property, and set_tests_properties()
# would replace it, not extend it. Headlessness does not need help either:
# the test preset and the CI jobs both export QT_QPA_PLATFORM=offscreen.
set(DEMO_SCREENSHOT_DIR "${CMAKE_CURRENT_BINARY_DIR}/screenshots")
file(MAKE_DIRECTORY "${DEMO_SCREENSHOT_DIR}")

# Qt5_VERSION is the shim's variable and is set for both, see cmake/FindQt5.cmake.
if (Qt5_VERSION VERSION_LESS 6.0.0)
    set(DEMO_GOLDEN_QT "Qt5")
else()
    set(DEMO_GOLDEN_QT "Qt6")
endif()

set(DEMO_SCREENSHOT_DEFINES)
set(DEMO_SCREENSHOT_TARGETS)

if (TARGET RibbonDemo)
  set(DEMO_SCREENSHOT "${DEMO_SCREENSHOT_DIR}/ribbondemo.png")
  # One golden cannot serve both Qt majors: the same demo measures 1280x800 on
  # Qt 6 and 1333x833 on Qt 5, so a Qt 5 capture fails the size check before a
  # single pixel is compared (task C2). Splitting by Qt major also gives the
  # honest answer for a major that has no golden at all - "no golden for this
  # platform", a skip - instead of a failure that reads like a regression.
  set(DEMO_GOLDEN "${QTCANPOOL_TESTS_DIR}/demo/golden/${CMAKE_SYSTEM_NAME}/${DEMO_GOLDEN_QT}/ribbondemo.png")
  list(APPEND DEMO_SCREENSHOT_DEFINES
      DEMO_RIBBON_EXECUTABLE="$<TARGET_FILE:RibbonDemo>"
      DEMO_SCREENSHOT_PATH="${DEMO_SCREENSHOT}"
      DEMO_GOLDEN_PATH="${DEMO_GOLDEN}"
  )
  list(APPEND DEMO_SCREENSHOT_TARGETS RibbonDemo)
endif()

if (TARGET DockDemo)
  set(DEMO_DOCK_SCREENSHOT "${DEMO_SCREENSHOT_DIR}/dockdemo.png")
  set(DEMO_DOCK_GOLDEN "${QTCANPOOL_TESTS_DIR}/demo/golden/${CMAKE_SYSTEM_NAME}/${DEMO_GOLDEN_QT}/dockdemo.png")
  list(APPEND DEMO_SCREENSHOT_DEFINES
      DEMO_DOCK_EXECUTABLE="$<TARGET_FILE:DockDemo>"
      DEMO_DOCK_SCREENSHOT_PATH="${DEMO_DOCK_SCREENSHOT}"
      DEMO_DOCK_GOLDEN_PATH="${DEMO_DOCK_GOLDEN}"
  )
  list(APPEND DEMO_SCREENSHOT_TARGETS DockDemo)
endif()

if (DEMO_SCREENSHOT_DEFINES)
  add_qtc_test(tst_demoscreenshot
      DEPENDS Qt5::Gui
      DEFINES ${DEMO_SCREENSHOT_DEFINES}
      SOURCES "${QTCANPOOL_TESTS_DIR}/demo/auto/tst_demoscreenshot.cpp"
  )
  # Running the test is what builds it, and the test launches the demos: without
  # these dependencies they may simply not exist yet.
  foreach (demo ${DEMO_SCREENSHOT_TARGETS})
    add_dependencies(tst_demoscreenshot ${demo})
  endforeach()
  set_tests_properties(tst_demoscreenshot PROPERTIES
      LABELS visual
      TIMEOUT 300
  )
endif()
