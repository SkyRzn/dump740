cmake_minimum_required(VERSION 2.6)

project (dump740 C)

set(VERSION_INFO_MAJOR_VERSION 0)
set(VERSION_INFO_MINOR_VERSION 1)
set(VERSION_INFO_PATCH_VERSION 11)

add_definitions(-DVERSION_MAJOR=${VERSION_INFO_MAJOR_VERSION})
add_definitions(-DVERSION_MINOR=${VERSION_INFO_MINOR_VERSION})
add_definitions(-DVERSION_PATCH=${VERSION_INFO_PATCH_VERSION})
add_definitions("-O3 -Wall")

file(GLOB SOURCES "*.c")

add_executable(dump740 ${SOURCES})

include(FindPkgConfig)
pkg_check_modules(LIBRTLSDR librtlsdr)
include_directories(${LIBRTLSDR_INCLUDE_DIRS})
target_link_libraries (dump740 pthread m ${LIBRTLSDR_LIBRARIES})

