!isEmpty(QTNCONFIG_PRI_INCLUDED):warning("qtitanconfig.pri already included")
QTNCONFIG_PRI_INCLUDED = 1

QTITANDIR = $$quote($$(QTITANDIR))
isEmpty(QTITANDIR):QTITANDIR = $$quote($$PWD/../../)
