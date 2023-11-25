TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    ffmpegdemo

win32 {
    win32-g++ {
        SUBDIRS += \
            boostdemo
    }
}
