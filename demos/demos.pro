TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    qcanpool \
    qxapp \
    qxwindow

DEMO_OTHERS_ENABLE = 0
equals(DEMO_OTHERS_ENABLE, 1) {
    SUBDIRS += helloworld
}
