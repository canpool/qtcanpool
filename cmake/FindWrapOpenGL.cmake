# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause
#
# Local override of Qt's bundled FindWrapOpenGL.cmake.
#
# Qt releases before 6.9 unconditionally link the deprecated AGL (Apple Graphics
# Library) framework when building on macOS: the module resolves AGL via
# find_library() and falls back to the literal "-framework AGL" flag. AGL was
# removed from the macOS SDK in macOS 26 (Tahoe), so linking fails with
#
#     ld: framework 'AGL' not found
#
# for every target that (indirectly) depends on Qt6::Gui - which is all of our
# GUI libraries and tests. Qt deleted the whole AGL block in 6.9; this file is a
# verbatim copy of the Qt 6.8 module with exactly that block removed, matching
# the upstream 6.9 fix line for line.
#
# The top-level CMakeLists.txt prepends our cmake/ directory to
# CMAKE_MODULE_PATH, so this module takes precedence over the copy shipped
# inside Qt. This is a no-op on Windows and Linux (the AGL block is APPLE only).

# We can't create the same interface imported target multiple times, CMake will complain if we do
# that. This can happen if the find_package call is done in multiple different subdirectories.
if(TARGET WrapOpenGL::WrapOpenGL)
    set(WrapOpenGL_FOUND ON)
    return()
endif()

set(WrapOpenGL_FOUND OFF)

find_package(OpenGL ${WrapOpenGL_FIND_VERSION})

if (OpenGL_FOUND)
    set(WrapOpenGL_FOUND ON)

    add_library(WrapOpenGL::WrapOpenGL INTERFACE IMPORTED)
    if(APPLE)
        # CMake 3.27 and older:
        # On Darwin platforms FindOpenGL sets IMPORTED_LOCATION to the absolute path of the library
        # within the framework. This ends up as an absolute path link flag, which we don't want,
        # because that makes our .prl files un-relocatable.
        # Extract the framework path instead, and use that in INTERFACE_LINK_LIBRARIES,
        # which CMake ends up transforming into a relocatable -framework flag.
        # See https://gitlab.kitware.com/cmake/cmake/-/issues/20871 for details.
        #
        # CMake 3.28 and above:
        # IMPORTED_LOCATION is the absolute path the the OpenGL.framework folder.
        get_target_property(__opengl_fw_lib_path OpenGL::GL IMPORTED_LOCATION)
        if(__opengl_fw_lib_path AND NOT __opengl_fw_lib_path MATCHES "/([^/]+)\\.framework$")
            get_filename_component(__opengl_fw_path "${__opengl_fw_lib_path}" DIRECTORY)
        endif()

        if(NOT __opengl_fw_path)
            # Just a safety measure in case if no OpenGL::GL target exists.
            set(__opengl_fw_path "-framework OpenGL")
        endif()

        target_link_libraries(WrapOpenGL::WrapOpenGL INTERFACE ${__opengl_fw_path})
    else()
        target_link_libraries(WrapOpenGL::WrapOpenGL INTERFACE OpenGL::GL)
    endif()
elseif(UNIX AND NOT APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "Integrity")
    # Requesting only the OpenGL component ensures CMake does not mark the package as
    # not found if neither GLX nor libGL are available. This allows finding OpenGL
    # on an X11-less Linux system.
    find_package(OpenGL ${WrapOpenGL_FIND_VERSION} COMPONENTS OpenGL)
    if (OpenGL_FOUND)
        set(WrapOpenGL_FOUND ON)
        add_library(WrapOpenGL::WrapOpenGL INTERFACE IMPORTED)
        target_link_libraries(WrapOpenGL::WrapOpenGL INTERFACE OpenGL::OpenGL)
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WrapOpenGL DEFAULT_MSG WrapOpenGL_FOUND)
