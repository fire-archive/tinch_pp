project( tinch_pp )

include(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_VERSION "0.1")

cmake_minimum_required(VERSION 2.8)

add_subdirectory(impl)
add_subdirectory(test)
add_subdirectory(tinch_pp)

include(CPack)