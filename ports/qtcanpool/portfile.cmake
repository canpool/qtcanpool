# Skeleton port for the qtcanpool 3.0 CMake package.
#
# NOTE: this port has NOT been validated in CI -- building qtbase from source
# is a multi-hour job that does not fit the project's CI budget. It is offered
# as a starting point for the community. Fill in the real SHA512 once the
# v3.0.0 tag is pushed, then run `vcpkg install qtcanpool` on a machine that
# can afford to build Qt.

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO canpool/qtcanpool
    REF "v${VERSION}"
    SHA512 0 # TODO: real archive hash, fill in after tagging v3.0.0
    HEAD_REF master
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DWITH_DEMOS=OFF
        -DWITH_TESTS=OFF
        -DWITH_DOCS=OFF
        -DBUILD_WITH_PCH=OFF
)

vcpkg_cmake_install()

# qtcanpool installs its public headers and its CMake package
# (QtCanpoolConfig.cmake / QtCanpoolTargets.cmake) into an EXCLUDE_FROM_ALL
# "Devel" component, which the plain install above deliberately skips. Install
# that component explicitly for every configuration that was built.
foreach(_cfg rel dbg)
    set(_build "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-${_cfg}")
    if(EXISTS "${_build}")
        if(_cfg STREQUAL "rel")
            set(_prefix "${CURRENT_PACKAGES_DIR}")
            set(_config Release)
        else()
            set(_prefix "${CURRENT_PACKAGES_DIR}/debug")
            set(_config Debug)
        endif()
        vcpkg_execute_required_process(
            COMMAND "${CMAKE_COMMAND}" --install "${_build}"
                    --config "${_config}" --component Devel --prefix "${_prefix}"
            WORKING_DIRECTORY "${_build}"
            LOGNAME "install-devel-${_cfg}"
        )
    endif()
endforeach()

vcpkg_cmake_config_fixup(
    PACKAGE_NAME QtCanpool
    CONFIG_PATH lib/cmake/QtCanpool
)

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share"
)

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
