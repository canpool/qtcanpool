# qtcanpool tests (CMake / CTest)
#
# qmake 侧入口为 tests/tests.pro（自 3.0 起冻结）。
# 此处为 CMake 主构建下的测试入口，仅在 -DWITH_TESTS=ON 时生效。
#
# 复用框架提供的 add_qtc_test()：自动注册 CTest、处理 rpath / automoc /
# include 路径（库的 PUBLIC include 为 src/libs，故测试可直接
# #include "qxribbon/xxx.h"、"qxdock/xxx.h"）。

set(QTCANPOOL_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

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
# 无库依赖的最小示例测试（QTEST_MAIN 自带入口）
add_qtc_test(tst_qstring
    DEPENDS Qt5::Core
    SOURCES "${QTCANPOOL_TESTS_DIR}/helloworld/qstring/tst_qstring.cpp"
)
add_qtc_test(tst_qlineedit
    DEPENDS Qt5::Widgets
    SOURCES "${QTCANPOOL_TESTS_DIR}/helloworld/qlineedit/tst_qlineedit.cpp"
)
