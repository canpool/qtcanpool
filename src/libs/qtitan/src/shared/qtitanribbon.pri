
QTITAN_ROOT = $$PWD/../..

include($$QTITAN_ROOT/src/shared/platform.pri)

QTITAN_LIB_PATH = $$QTITAN_ROOT/bin$$QTITAN_LIB_PREFIX
INCLUDEPATH += $$QTITAN_ROOT/include

QTN_COMPONENT_VERSION = $$fromfile($$QTITAN_ROOT/src/ribbon/ribbon.pri, QTN_COMPONENT_VERSION)
DEFINES += QTN_VERSION_RIBBON=$$QTN_COMPONENT_VERSION


win32 {
	win32-g++ {
		LIBS += -L$$QTITAN_LIB_PATH
		CONFIG(debug, debug|release):LIBS += -lqtnribbond4
			else:LIBS += -lqtnribbon4
	}
	else {
		CONFIG(debug, debug|release):LIBS += $$QTITAN_LIB_PATH/qtnribbond4.lib
			else:LIBS += $$QTITAN_LIB_PATH/qtnribbon4.lib
	}
}

unix {
    LIBS += -L$$QTITAN_LIB_PATH
    CONFIG(debug, debug|release):LIBS += -lqtnribbond
        else:LIBS += -lqtnribbon
}

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets
}
