# ---------------------------------------------------------------------------
# The QtCanpool documentation site.
#
# This directory is a CMake project of its own, so the site can be built with
# nothing but Doxygen (and optionally Graphviz) installed:
#
#     cmake -S doc -B build-docs
#     cmake --build build-docs --target docs
#
# It is also pulled in by the top-level build when WITH_DOCS is enabled, where
# the same target becomes available next to the libraries. Qt is not required in
# either mode: the API is read from the headers, never compiled.
#
# Useful cache variables:
#
#     -DDOXYGEN_EXECUTABLE=<path>     pick a specific Doxygen
#     -DDOXYGEN_PROJECT_NUMBER=<ver>  override the version in the page footer
#     -DDOXYGEN_WARN_AS_ERROR=YES     fail the build on any warning
#     -DDOXYGEN_OUTPUT_LANGUAGE=<lang>  localise the generated pages
# ---------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(QtCanpoolDocs LANGUAGES NONE)
  set(QTCANPOOL_DOC_STANDALONE TRUE)
  option(WITH_DOCS "Build the documentation site" ON)
else()
  set(QTCANPOOL_DOC_STANDALONE FALSE)
endif()

get_filename_component(QTCANPOOL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

# The branding file is the single source of truth for the project name and
# version; the top level already includes it, re-including only re-sets the same
# values in this directory scope.
if (EXISTS "${QTCANPOOL_ROOT}/cmake/QtCanpoolBranding.cmake")
  include("${QTCANPOOL_ROOT}/cmake/QtCanpoolBranding.cmake")
endif()

set(DOXYGEN_PROJECT_NAME "QtCanpool" CACHE STRING "Project name shown in the documentation")
if (QTPROJECT_DISPLAY_VERSION)
  set(_doc_number "${QTPROJECT_DISPLAY_VERSION}")
else()
  set(_doc_number "unversioned")
endif()
set(DOXYGEN_PROJECT_NUMBER "${_doc_number}" CACHE STRING "Version shown in the documentation")

find_program(DOXYGEN_EXECUTABLE NAMES doxygen)
if (NOT DOXYGEN_EXECUTABLE)
  # Configured on its own, a missing Doxygen is the whole point of the invocation
  # and therefore an error; as part of the main build it only disables a target.
  if (QTCANPOOL_DOC_STANDALONE)
    message(FATAL_ERROR "Doxygen was not found. Install it, or pass -DDOXYGEN_EXECUTABLE=<path>.")
  endif()
  message(WARNING "Doxygen was not found: the documentation target is not available.")
  return()
endif()

execute_process(
  COMMAND "${DOXYGEN_EXECUTABLE}" --version
  OUTPUT_VARIABLE _doxygen_version_raw
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)
# Recent releases print "1.13.2 (a1b2c3d)" or "1.9.8"; keep the numeric part only.
string(REGEX MATCH "^[0-9]+\\.[0-9]+(\\.[0-9]+)?" DOXYGEN_VERSION "${_doxygen_version_raw}")
if (NOT DOXYGEN_VERSION)
  set(DOXYGEN_VERSION "unknown")
endif()

find_program(DOXYGEN_DOT_EXECUTABLE NAMES dot)

# ------------------------------------------------------------------ settings --

set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(DOXYGEN_OUTPUT_LANGUAGE "Chinese" CACHE STRING "Language of the generated documentation")
set(DOXYGEN_STRIP_FROM_PATH "${QTCANPOOL_ROOT}")
set(DOXYGEN_STRIP_FROM_INC_PATH "${QTCANPOOL_ROOT}/src/libs")
set(DOXYGEN_INCLUDE_PATH "${QTCANPOOL_ROOT}/src/libs")
set(DOXYGEN_IMAGE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/pics")
set(DOXYGEN_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/pages/index.md")
set(DOXYGEN_EXTRA_STYLESHEET "${CMAKE_CURRENT_SOURCE_DIR}/doxygen.css")
set(DOXYGEN_WARN_LOGFILE "${CMAKE_CURRENT_BINARY_DIR}/doxygen-warnings.log")
set(DOXYGEN_WARN_AS_ERROR "NO" CACHE STRING "Treat Doxygen warnings as errors (NO, YES or FAIL_ON_WARNINGS)")

if (DOXYGEN_DOT_EXECUTABLE)
  set(DOXYGEN_HAVE_DOT "YES")
else()
  set(DOXYGEN_HAVE_DOT "NO")
endif()

# src/libs carries the public API; pages carries the guides. Both are plain file
# lists in the configured Doxyfile, so the list separator has to be a space.
set(_doc_input "${QTCANPOOL_ROOT}/src/libs" "${CMAKE_CURRENT_SOURCE_DIR}/pages")
string(REPLACE ";" " " DOXYGEN_INPUT "${_doc_input}")
set(DOXYGEN_EXCLUDE_PATTERNS "*/moc_* */ui_* */CMakeFiles/* */.git/*")

# ------------------------------------------------------------------- targets --

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
               "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

set(_doc_target "docs")
if (TARGET ${_doc_target})
  # The inherited qdoc helper in cmake/QtCanpoolDocumentation.cmake claims the
  # same name; step aside rather than fight over it.
  set(_doc_target "docs-html")
  message(STATUS "Target 'docs' is taken; the documentation site is built with '${_doc_target}'.")
endif()

add_custom_target(${_doc_target}
  COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
  COMMENT "Generating the QtCanpool documentation site"
  VERBATIM
)

message(STATUS
  "QtCanpool documentation: 'cmake --build . --target ${_doc_target}' -> ${DOXYGEN_OUTPUT_DIR}/html "
  "(Doxygen ${DOXYGEN_VERSION}, Graphviz ${DOXYGEN_HAVE_DOT})")
