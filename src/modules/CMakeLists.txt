# QtCanpool business module samples (plan 4.0, task C2).
#
# A module is a plugin, not a library: it is built with add_qtc_plugin(), it
# links qxplugin and nothing above it, and it is loaded through QxPluginManager.
# Nothing in the framework depends on anything in here - these exist to be read
# as much as to be run, and to give D1's shell something to be assembled from.
#
# The subdirectories are grouped by the level convention of
# src/plugins/CMakeLists.txt: a module may only depend on modules of a lower
# level. The level is also what its PLUGIN_DEPENDS edge means, so the metadata
# and this file say the same thing.

option(WITH_MODULES "Build the plugin module samples" ON)

if (NOT WITH_MODULES)
  return()
endif()

# Level 1: no dependency on another module.
add_subdirectory(output)
add_subdirectory(notebook)

# Level 2: depends on a Level 1 module. filetree reports what it opens to the
# output panel, so it requires output and the manager guarantees output is
# initialized first.
add_subdirectory(filetree)

# broken/ is deliberately absent: it is a module with a dependency that cannot
# be satisfied, kept as source for the documentation and screenshots. See its
# README for why it must not enter the build.
