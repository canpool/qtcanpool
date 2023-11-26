TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    ffmpegdemo

win32 {
    win32-g++ {
        SUBDIRS += \
            boostdemo
    }
    win32-msvc {
        exists($$PWD/occQt/occQt.pro) {
            SUBDIRS += \
                occQt
        }
    }
}
