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

# ------------------------------------------------------------------- qxapp --
# qxapp is the application framework, so its test links the whole stack: the
# shell builds a ribbon window with a dock area and a theme.
file(GLOB QXAPP_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxapp/auto/tst_*.cpp")
add_qtc_test(tst_qxapp
    DEPENDS qxapp
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxapp/auto/main.cpp"
        ${QXAPP_TEST_SOURCES}
)

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
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxribbon/auto/main.cpp"
        ${QXRIBBON_TEST_SOURCES}
)

# ------------------------------------------------------------------ qxdock --
file(GLOB QXDOCK_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxdock/auto/tst_*.cpp")
add_qtc_test(tst_qxdock
    DEPENDS qxdock
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxdock/auto/main.cpp"
        ${QXDOCK_TEST_SOURCES}
)

# -------------------------------------------------------------- qxwindow --
# qxwindow is platform-heavy, so this suite sticks to the backend-independent
# bookkeeping the shared WindowContext does. See the test file for the reasoning.
file(GLOB QXWINDOW_TEST_SOURCES CONFIGURE_DEPENDS "${QTCANPOOL_TESTS_DIR}/qxwindow/auto/tst_*.cpp")
add_qtc_test(tst_qxwindow
    DEPENDS qxwindow
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxwindow/auto/main.cpp"
        ${QXWINDOW_TEST_SOURCES}
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
# The demo renders itself offscreen through its own --screenshot switch, which
# keeps the window driving out of the test, and the capture is compared with a
# golden image. The golden is per platform (tests/demo/golden/<CMAKE_SYSTEM_NAME>/)
# because a screenshot is the one artefact here that legitimately differs between
# machines - same code, a different font rasteriser, different pixels - and the
# comparison is opt-in (QTCANPOOL_VISUAL_BASELINE=1) because it is only
# meaningful between the machine that produced the golden and the same machine
# later; CI, with its own fonts, must never judge one.
#
# Registered with the label "visual" so it can be filtered out of a run.
# NOTE: no ENVIRONMENT here - add_qtc_test() bakes the configure-time PATH
# (MinGW runtime + Qt DLLs) into that property, and set_tests_properties()
# would replace it, not extend it. Headlessness does not need help either:
# the test preset and the CI jobs both export QT_QPA_PLATFORM=offscreen.
if (TARGET RibbonDemo)
  set(DEMO_SCREENSHOT_DIR "${CMAKE_CURRENT_BINARY_DIR}/screenshots")
  file(MAKE_DIRECTORY "${DEMO_SCREENSHOT_DIR}")
  set(DEMO_SCREENSHOT "${DEMO_SCREENSHOT_DIR}/ribbondemo.png")
  set(DEMO_GOLDEN "${QTCANPOOL_TESTS_DIR}/demo/golden/${CMAKE_SYSTEM_NAME}/ribbondemo.png")

  add_qtc_test(tst_demoscreenshot
      DEPENDS Qt5::Gui
      DEFINES
          DEMO_RIBBON_EXECUTABLE="$<TARGET_FILE:RibbonDemo>"
          DEMO_SCREENSHOT_PATH="${DEMO_SCREENSHOT}"
          DEMO_GOLDEN_PATH="${DEMO_GOLDEN}"
      SOURCES "${QTCANPOOL_TESTS_DIR}/demo/auto/tst_demoscreenshot.cpp"
  )
  # Running the test is what builds it, and the test launches the demo: without
  # this dependency the demo may simply not exist yet.
  add_dependencies(tst_demoscreenshot RibbonDemo)
  set_tests_properties(tst_demoscreenshot PROPERTIES
      LABELS visual
      TIMEOUT 300
  )
endif()
