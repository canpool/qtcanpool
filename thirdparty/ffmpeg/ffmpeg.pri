# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0

FFMPEG_TOPDIR = $$PWD/../..
FFMPEG_ROOT = $$FFMPEG_TOPDIR/../3rdparty/ffmpeg/ffmpeg-6.1-full_build-shared
FFMPEG_ROOT = $$absolute_path($$FFMPEG_ROOT)

!exists($$FFMPEG_ROOT) {
    error("$$FFMPEG_ROOT: No such directory")
}

include($$FFMPEG_TOPDIR/qtproject.pri)

FFMPEG_INC_PATH = $$FFMPEG_ROOT/include
FFMPEG_LIB_PATH = $$FFMPEG_ROOT/lib
FFMPEG_DLL_PATH = $$FFMPEG_ROOT/bin

INCLUDEPATH += $$FFMPEG_INC_PATH
LIBS += -L$$FFMPEG_LIB_PATH

win32 {
    win32-g++ {
        # mingw
        LIBS += -llibavcodec.dll -llibavdevice.dll -llibavfilter.dll -llibavformat.dll \
                -llibavutil.dll -llibpostproc.dll -llibswresample.dll -llibswscale.dll
    } else {
        # win32-msvc
        LIBS += -lavcodec avdevice.lib avfilter.lib avformat.lib avutil.lib \
                postproc.lib swresample.lib swscale.lib
    }
}

ffmpeg_dll_files = $$files($$FFMPEG_DLL_PATH/*, false)
for(file, ffmpeg_dll_files):contains(file, ".*\\.dll$"):!exists($$file/*): \
    STATIC_FILES += $$file

STATIC_BASE = $$FFMPEG_DLL_PATH
STATIC_INSTALL_BASE = $$INSTALL_DATA_PATH
