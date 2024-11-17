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
