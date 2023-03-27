include(../../qtproject.pri)

TEMPLATE  = subdirs

SUBDIRS   = \
    coreplugin

for(p, SUBDIRS) {
    QTC_PLUGIN_DEPENDS =
    include($$p/$${p}_dependencies.pri)
    pv = $${p}.depends
    $$pv = $$QTC_PLUGIN_DEPENDS
}

QMAKE_EXTRA_TARGETS += deployqt # dummy
