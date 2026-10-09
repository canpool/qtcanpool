# qtcanpool tests (CMake / CTest)
#
# The qmake entry point is tests/tests.pro (frozen since 3.0). This file is the
# entry point of the CMake build and only takes effect with -DWITH_TESTS=ON.
#
# It reuses add_qtc_test() from the framework, which registers the test with
# CTest and takes care of rpath / automoc / include paths (the public include
# directory of every library is src/libs, so a test can simply
# #include "qxribbon/xxx.h" or "qxdock/xxx.h").

set(QTCANPOOL_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# ------------------------------------------------------------------- qxcore --
file(GLOB QXCORE_TEST_SOURCES "${QTCANPOOL_TESTS_DIR}/qxcore/auto/tst_*.cpp")
add_qtc_test(tst_qxcore
    DEPENDS qxcore
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxcore/auto/main.cpp"
        ${QXCORE_TEST_SOURCES}
)

# ---------------------------------------------------------------- qxtheme --
file(GLOB QXTHEME_TEST_SOURCES "${QTCANPOOL_TESTS_DIR}/qxtheme/auto/tst_*.cpp")
add_qtc_test(tst_qxtheme
    DEPENDS qxtheme
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxtheme/auto/main.cpp"
        ${QXTHEME_TEST_SOURCES}
)

# ---------------------------------------------------------------- qxribbon --
file(GLOB QXRIBBON_TEST_SOURCES "${QTCANPOOL_TESTS_DIR}/qxribbon/auto/tst_*.cpp")
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
file(GLOB QXDOCK_TEST_SOURCES "${QTCANPOOL_TESTS_DIR}/qxdock/auto/tst_*.cpp")
add_qtc_test(tst_qxdock
    DEPENDS qxdock
    INCLUDES "${QTCANPOOL_TESTS_DIR}/common"
    SOURCES
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.cpp"
        "${QTCANPOOL_TESTS_DIR}/common/tst_global.h"
        "${QTCANPOOL_TESTS_DIR}/qxdock/auto/main.cpp"
        ${QXDOCK_TEST_SOURCES}
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
