# This pri file is used by the template project to reference
# qtcanpool library or plugin source code
#
# Usage: config.pri include this file after defining QTCANPOOL_DIR
#
# The replace function is then called in the pro file, such as:
#
# - reference qcanpool library:
#   include($$qtcLibrarySource(qcanpool))
#
# - reference coreplugin plugin:
#   include($$qtcPluginSource(coreplugin))
#
# NOTE:
# - the library must provide the LIBNAME-lib.pri file
# - the plugin must provide the PLUGINNAME-plugin.pri file
#

defineReplace(qtcLibrarySource) {
    LIB_PRI = $$absolute_path($$QTCANPOOL_DIR/src/libs/$$1/$$1-lib.pri)
    return($$LIB_PRI)
}

defineReplace(qtcPluginSource) {
    PLUGIN_PRI = $$absolute_path($$QTCANPOOL_DIR/src/plugins/$$1/$$1-plugin.pri)
    return($$PLUGIN_PRI)
}
