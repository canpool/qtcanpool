add_subdirectory(libs)
add_subdirectory(shared)
add_subdirectory(plugins)

install(
  FILES
    ${PROJECT_SOURCE_DIR}/LICENSE
    ${PROJECT_SOURCE_DIR}/LICENSE.NOTES.md
    ${PROJECT_SOURCE_DIR}/README.md
  DESTINATION ${IDE_DATA_PATH}
  COMPONENT Devel EXCLUDE_FROM_ALL
)

install(
  DIRECTORY
    ${PROJECT_SOURCE_DIR}/scripts
  DESTINATION ${IDE_DATA_PATH}
  COMPONENT Devel EXCLUDE_FROM_ALL
  USE_SOURCE_PERMISSIONS
)

install(EXPORT QtCanpool
  DESTINATION ${IDE_CMAKE_INSTALL_PATH}/QtCanpool
  COMPONENT Devel EXCLUDE_FROM_ALL
  NAMESPACE QtCanpool::
  FILE QtCanpoolTargets.cmake
)

file(WRITE ${CMAKE_BINARY_DIR}/cmake/QtCanpoolConfig.cmake "
\# add module path for special FindQt5.cmake that considers Qt6 too
list(APPEND CMAKE_MODULE_PATH \${CMAKE_CURRENT_LIST_DIR})

\# force plugins to same path naming conventions as Qt Canpool
\# otherwise plugins will not be found
if(UNIX AND NOT APPLE)
  include(GNUInstallDirs)
  set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
  set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
  set(CMAKE_INSTALL_LIBEXECDIR ${CMAKE_INSTALL_LIBEXECDIR})
  set(CMAKE_INSTALL_DATAROOTDIR ${CMAKE_INSTALL_DATAROOTDIR})
endif()

include(CMakeFindDependencyMacro)
find_dependency(Qt5 ${IDE_QT_VERSION_MIN}
  COMPONENTS Concurrent Core Gui Widgets Core5Compat REQUIRED
)

if (NOT QTPROJECT_VERSION)
  include(\${CMAKE_CURRENT_LIST_DIR}/QtCanpoolBranding.cmake)
endif()

if (NOT DEFINED add_qtc_plugin)
  include(\${CMAKE_CURRENT_LIST_DIR}/QtCanpoolAPI.cmake)
endif()

if (NOT DEFINED add_translation_targets)
  include(\${CMAKE_CURRENT_LIST_DIR}/QtCanpoolTranslations.cmake)
endif()

if (NOT DEFINED add_qtc_documentation)
  include(\${CMAKE_CURRENT_LIST_DIR}/QtCanpoolDocumentation.cmake)
endif()

if (NOT TARGET QtCanpool::Core)
  include(\${CMAKE_CURRENT_LIST_DIR}/QtCanpoolTargets.cmake)
endif()
")

# Local build export
export(EXPORT QtCanpool
  NAMESPACE QtCanpool::
  FILE ${CMAKE_BINARY_DIR}/cmake/QtCanpoolTargets.cmake)

file(COPY
  ${IDE_BRANDING_FILE}
  ${PROJECT_SOURCE_DIR}/cmake/QtCanpoolTranslations.cmake
  ${PROJECT_SOURCE_DIR}/cmake/QtCanpoolDocumentation.cmake
  ${PROJECT_SOURCE_DIR}/cmake/QtCanpoolAPI.cmake
  ${PROJECT_SOURCE_DIR}/cmake/QtCanpoolAPIInternal.cmake
  ${PROJECT_SOURCE_DIR}/cmake/FindQt5.cmake
  ${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in
  ${PROJECT_SOURCE_DIR}/cmake/QtcSeparateDebugInfo.cmake
  ${PROJECT_SOURCE_DIR}/cmake/QtcSeparateDebugInfo.Info.plist.in
  DESTINATION ${CMAKE_BINARY_DIR}/cmake
)

# Devel package install
install(
  FILES
    ${IDE_BRANDING_FILE}
    ${PROJECT_SOURCE_DIR}/cmake/QtCanpoolTranslations.cmake
    ${PROJECT_SOURCE_DIR}/cmake/QtCanpoolDocumentation.cmake
    ${PROJECT_SOURCE_DIR}/cmake/QtCanpoolAPI.cmake
    ${PROJECT_SOURCE_DIR}/cmake/QtCanpoolAPIInternal.cmake
    ${PROJECT_SOURCE_DIR}/cmake/FindQt5.cmake
    ${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in
    ${PROJECT_SOURCE_DIR}/cmake/QtcSeparateDebugInfo.cmake
    ${PROJECT_SOURCE_DIR}/cmake/QtcSeparateDebugInfo.Info.plist.in
    ${CMAKE_BINARY_DIR}/cmake/QtCanpoolConfig.cmake
  DESTINATION ${IDE_CMAKE_INSTALL_PATH}/QtCanpool
  COMPONENT Devel EXCLUDE_FROM_ALL
)
