include(../qtproject.pri)

TEMPLATE  = subdirs
CONFIG   += ordered


SUBDIRS += \
    libs \
    tools \


TRANSLATIONS += app/resource/qm/lang_zh_CN.ts \
                app/resource/qm/lang_en_US.ts \
#                app/resource/qm/qt5.8.0/qt_zh_CN.ts
