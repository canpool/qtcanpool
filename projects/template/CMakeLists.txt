##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# A QtCanpool application, deliberately standalone: it knows nothing about the
# QtCanpool sources - no add_subdirectory, no relative paths - and consumes an
# *installed* SDK through find_package, which is what a downstream project
# looks like.

cmake_minimum_required(VERSION 3.16)

project(qxtemplate
  VERSION 0.1.0
  DESCRIPTION "qxtemplate - a QtCanpool application"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt's code generators: AUTORCC picks up resources/app.qrc below, and AUTOMOC is
# what turns a Q_OBJECT in your own classes into a compilable unit.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Point CMAKE_PREFIX_PATH at the QtCanpool SDK prefix and at Qt; QtCanpoolConfig
# pulls in the Qt it was built against, so nothing else is needed here.
find_package(QtCanpool REQUIRED)

# WIN32 only suppresses the console window on Windows; it is ignored elsewhere.
add_executable(qxtemplate WIN32
  main.cpp
  resources/app.qrc
)

# QtCanpool::qxapp re-exports the ribbon, the dock system, the window agent, the
# theme engine and their Qt dependencies, so one link line is enough.
target_link_libraries(qxtemplate PRIVATE QtCanpool::qxapp)

# A library of your own goes the same way: give it a subdirectory with its own
# CMakeLists.txt, add_subdirectory() it here, then link it. Use PUBLIC there
# only if it exposes QtCanpool types in its public headers.
