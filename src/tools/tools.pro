TEMPLATE = subdirs

SUBDIRS = \
    liccreator

win32 {
#    SUBDIRS += qtcdebugger \
#        wininterrupt \
#        winrtdebughelper
}

mac {
    SUBDIRS += iostool
}

isEmpty(LLVM_INSTALL_DIR):LLVM_INSTALL_DIR=$$(LLVM_INSTALL_DIR)
exists($$LLVM_INSTALL_DIR) {
    SUBDIRS += clangbackend
#    SUBDIRS += clangrefactoringbackend
}

isEmpty(BUILD_CPLUSPLUS_TOOLS):BUILD_CPLUSPLUS_TOOLS=$$(BUILD_CPLUSPLUS_TOOLS)
!isEmpty(BUILD_CPLUSPLUS_TOOLS) {
    SUBDIRS += cplusplus-ast2png \
        cplusplus-frontend \
        cplusplus-mkvisitor \
        cplusplus-update-frontend
}

QT_BREAKPAD_ROOT_PATH = $$(QT_BREAKPAD_ROOT_PATH)
!isEmpty(QT_BREAKPAD_ROOT_PATH) {
    SUBDIRS += qtcrashhandler
} else {
    linux-* {
        # Build only in debug mode.
        debug_and_release|CONFIG(debug, debug|release) {
            SUBDIRS += qtcreatorcrashhandler
        }
    }
}
