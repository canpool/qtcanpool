# [FROZEN] qmake support is frozen since 3.0 (read-only).
# CMake is the primary build system now; new features only go into CMake.
# See doc/ROADMAP-3.0.md for details.
TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    src \
    demos \
    examples \


TEST_BUILD_ENABLE = 1
equals(TEST_BUILD_ENABLE, 1) {
    SUBDIRS += tests
}
