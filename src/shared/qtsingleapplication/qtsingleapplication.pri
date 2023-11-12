INCLUDEPATH	+= $$PWD
DEPENDPATH      += $$PWD
HEADERS		+= $$PWD/qtsingleapplication.h $$PWD/qtlocalpeer.h
SOURCES		+= $$PWD/qtsingleapplication.cpp $$PWD/qtlocalpeer.cpp

QT *= network widgets

gotqtlockedfile = $$find(HEADERS, .*qtlockedfile.h)
# modified by maminjie <canpool@163.com> 2023-3-25 17:25:38
isEmpty(gotqtlockedfile):include($$PWD/../qtlockedfile/qtlockedfile.pri)


win32:contains(TEMPLATE, lib):contains(CONFIG, shared) {
    DEFINES += QT_QTSINGLEAPPLICATION_EXPORT=__declspec(dllexport)
}
