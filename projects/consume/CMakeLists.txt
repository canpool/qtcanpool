cmake_minimum_required(VERSION 3.16)

project(qxconsume LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This is the whole point of the project: consume an installed QtCanpool SDK
# through a plain find_package, with no knowledge of the build tree. Point
# CMAKE_PREFIX_PATH at the installation prefix (see README.md).
find_package(QtCanpool REQUIRED)

add_executable(qxconsume main.cpp)

target_link_libraries(qxconsume PRIVATE QtCanpool::qxapp)
