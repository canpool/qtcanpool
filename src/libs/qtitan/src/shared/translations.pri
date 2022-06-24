

exists($$LRELEASE) {

    defineReplace(prependAll) {
        for(a,$$1):result += $$2$${a}$$3
        return($$result)
    }

    defineTest(testFiles) {
        for(file, $$1) {
            !exists($$file):return(false)
        }
        return(true)
    }

    SUCCESS = false
    COMPONENT_TRANSLATIONS = $$prependAll(COMPONENT_LANGUAGES, $$COMPONENT_TRANSLATIONS_DIR/,.ts)

    wd = $$toNativeSeparators($$COMPONENT_SOURCE_TREE)
    sources = .
    lupdate_opts = -locations relative -no-ui-lines -no-sort

    for(file, COMPONENT_TRANSLATIONS) {
        lang = $$replace(file, .*/([^/]*)\\.ts, \\1)
        v = ts-$${lang}.commands
        $$v = cd $$wd && $$LUPDATE $$lupdate_opts $$sources -ts $$file
        QMAKE_EXTRA_TARGETS += ts-$$lang
		TRANSLATIONS += $$file
    }
    ts-all.commands = cd $$wd && $$LUPDATE $$lupdate_opts $$sources -ts $$COMPONENT_TRANSLATIONS

    QMAKE_EXTRA_TARGETS += ts-all

    SUCCESS = true

    if (contains(SUCCESS, true)) {
        updateqm.input = COMPONENT_TRANSLATIONS
        updateqm.output = $$COMPONENT_TRANSLATIONS_DIR/${QMAKE_FILE_BASE}.qm
        updateqm.commands = $$LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
        updateqm.name = LRELEASE ${QMAKE_FILE_IN}
        updateqm.CONFIG += no_link target_predeps
        QMAKE_EXTRA_COMPILERS += updateqm
    }
}

