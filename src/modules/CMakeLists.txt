# QtCanpool business module samples (plan 4.0, task C2).
#
# A module is a plugin, not a library: it is built with add_qtc_plugin(), it
# links qxplugin and nothing above it, and it is loaded through QxPluginManager.
# Nothing in the framework depends on anything in here - these exist to be read
# as much as to be run, and to give D1's shell something to be assembled from.
#
# The subdirectories are grouped by the level convention of
# src/plugins/CMakeLists.txt: a module may only depend on modules of a lower
# level. The level is also what a dependency edge means - PLUGIN_DEPENDS for a
# hard one, PLUGIN_RECOMMENDS for the soft kind - so the metadata and this file
# say the same thing.
#
# In a static configuration (QTC_STATIC_BUILD, which is what the wasm preset
# sets) add_qtc_plugin() produces a static library instead, and nothing imports
# it: a static plugin only reaches an application that names it in
# Q_IMPORT_PLUGIN. These targets are still built there - they have to compile -
# but the way they reach a static application is registerStaticPlugin(), which
# is what tst_qxplugin exercises. Runtime discovery is dynamic-only; see
# doc/design/4.0-TASKS.md, D1.

option(WITH_MODULES "Build the plugin module samples" ON)

if (NOT WITH_MODULES)
  return()
endif()

# Level 1: no dependency on another module.
add_subdirectory(output)
add_subdirectory(notebook)

# Level 2: depends on a Level 1 module, but only softly. filetree reports what
# it opens to the output panel and works without one, so it recommends output
# instead of requiring it: the manager still initializes output first when it is
# there, and filetree now starts when it is not.
add_subdirectory(filetree)

# broken/ is deliberately absent: it is a module with a dependency that cannot
# be satisfied, kept as source for the documentation and screenshots. See its
# README for why it must not enter the build.
