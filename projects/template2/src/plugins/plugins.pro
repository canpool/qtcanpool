include(../../qtproject.pri)

TEMPLATE  = subdirs

SUBDIRS   = \
    coreplugin \
    helloworld

for(p, SUBDIRS) {
    QTC_PLUGIN_DEPENDS =
    include($$p/$${p}_dependencies.pri)
    pv = $${p}.depends
    $$pv = $$QTC_PLUGIN_DEPENDS
}
