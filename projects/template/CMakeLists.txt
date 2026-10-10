##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# The CMake template: the starting point for a new project that builds against
# an *installed* QtCanpool SDK.
#
# It is deliberately standalone. It is not part of the QtCanpool build tree and
# it knows nothing about the sources - no add_subdirectory, no relative paths,
# only find_package. That is exactly what a downstream project looks like.
#
# This directory is the template and nothing else: a CMakeLists.txt, a main.cpp
# and a README.md. The qmake version that used to sit next to it was removed in
# 3.2 together with the qcanpool library it referenced. New projects should start
# from this file; see README.md for the two commands that build and run it.

cmake_minimum_required(VERSION 3.16)

project(qxtemplate
  VERSION 0.1.0
  DESCRIPTION "QtCanpool application template"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# A QtCanpool SDK has to be reachable: either install it to a prefix and put
# that prefix on CMAKE_PREFIX_PATH, or use the build tree of a superbuild.
# QtCanpoolConfig.cmake pulls in the Qt dependency it was built against.
find_package(QtCanpool REQUIRED)

# WIN32 only suppresses the console window on Windows; it is ignored elsewhere.
add_executable(qxtemplate WIN32
  main.cpp
)

# QtCanpool::qxapp carries the whole application stack: linking it re-exports
# the ribbon, the dock system, the window agent, the theme engine and their Qt
# dependencies (Core, Gui, Widgets), so a single link line is enough.
target_link_libraries(qxtemplate PRIVATE QtCanpool::qxapp)

# Add a local library the same way the qmake template did: create a subdirectory
# with its own CMakeLists.txt and link it here. Link QtCanpool targets PUBLIC
# there only if the library exposes QtCanpool types in its public headers.
