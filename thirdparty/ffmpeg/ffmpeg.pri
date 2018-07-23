
FFMPEG_ROOT = $$PWD

include($$FFMPEG_ROOT/../../qtproject.pri)

INCLUDEPATH += $$FFMPEG_ROOT/include

FFMPEG_LIB_PATH = $$FFMPEG_ROOT/lib
FFMPEG_DLL_PATH = $$FFMPEG_ROOT/dll
contains(QMAKE_HOST.arch, x86_64) {
    FFMPEG_LIB_PATH = $$FFMPEG_LIB_PATH/win64
    FFMPEG_DLL_PATH = $$FFMPEG_DLL_PATH/win64
}
contains(QMAKE_HOST.arch, x86) {
    FFMPEG_LIB_PATH = $$FFMPEG_LIB_PATH/win32
    FFMPEG_DLL_PATH = $$FFMPEG_DLL_PATH/win32
}

LIBS += -L$$FFMPEG_LIB_PATH

win32 {
    win32-g++ {
        LIBS += -llibavcodec.dll -llibavdevice.dll -llibavfilter.dll -llibavformat.dll \
               -llibavutil.dll -llibswresample.dll -llibswscale.dll -llibpostproc.dll
    }
    else {
        LIBS += -lavcodec avdevice.lib avfilter.lib avutil.lib \
                swresample.lib swscale.lib postproc.lib
    }
}

files = $$files($$FFMPEG_DLL_PATH/*, false)
for(file, files):contains(file, ".*\\.dll$"):!exists($$file/*): \
    STATIC_FILES += $$file

STATIC_BASE = $$FFMPEG_DLL_PATH
STATIC_INSTALL_BASE = $$INSTALL_DATA_PATH
