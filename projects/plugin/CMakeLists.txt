cmake_minimum_required(VERSION 3.16)

project(qxpluginacceptance LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# This is the whole point of the project: nothing below knows about the
# QtCanpool source tree. No CMAKE_MODULE_PATH into cmake/, no add_qtc_plugin(),
# no internal header - just the installed package, exactly like any downstream
# application would use it.
find_package(QtCanpool REQUIRED)

# 1) The plugin. A plugin is a module library whose Q_PLUGIN_METADATA carries
#    the QtCanpool IID and names a JSON file; moc embeds that file into the
#    shared object at build time, and that is what makes it discoverable later.
add_library(acceptance MODULE
    acceptanceplugin.cpp
    acceptanceplugin.h
    acceptance.json
)
target_link_libraries(acceptance PRIVATE QtCanpool::qxplugin)

# The manager looks for .so / .dll / .dylib / .bundle inside the directories it
# is given, so keep the plugin out of the directory the executables land in.
set(acceptance_plugin_dir "${CMAKE_CURRENT_BINARY_DIR}/plugins")
foreach(config IN ITEMS "" _DEBUG _RELEASE _RELWITHDEBINFO _MINSIZEREL)
    set_target_properties(acceptance PROPERTIES
        "LIBRARY_OUTPUT_DIRECTORY${config}" "${acceptance_plugin_dir}"
        "RUNTIME_OUTPUT_DIRECTORY${config}" "${acceptance_plugin_dir}"
    )
endforeach()

# 2) The host, small enough to read in one go. It loads the plugin above through
#    QxPluginManager and exits non-zero unless it was found and came up.
add_executable(qxpluginhost host.cpp)
target_link_libraries(qxpluginhost PRIVATE QtCanpool::qxplugin)
target_compile_definitions(qxpluginhost PRIVATE
    QX_ACCEPTANCE_PLUGIN_DIR="${acceptance_plugin_dir}"
)
