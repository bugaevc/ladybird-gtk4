cmake_minimum_required(VERSION 3.16)
project(ladybird-gtk4
  VERSION 1.0.0
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LAGOM_SOURCE_DIR "${SERENITY_SOURCE_DIR}/Meta/Lagom")
set(LAGOM_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Lagom")

include("${SERENITY_SOURCE_DIR}/Meta/CMake/lagom_compile_options.cmake")

set(BUILD_LAGOM ON CACHE INTERNAL "")
set(ENABLE_LAGOM_LIBWEB ON CACHE INTERNAL "")
# We set EXCLUDE_FROM_ALL to make sure that only required Lagom libraries are built
add_subdirectory("${LAGOM_SOURCE_DIR}" "${LAGOM_BINARY_DIR}" EXCLUDE_FROM_ALL)
# FIXME: Why doesn't Lagom do this?
include_directories(
    "${SERENITY_SOURCE_DIR}"
    "${SERENITY_SOURCE_DIR}/Userland/Libraries"
    "${SERENITY_SOURCE_DIR}/Userland/Applications"
    "${SERENITY_SOURCE_DIR}/Userland/Services"
    "${LAGOM_BINARY_DIR}"
    "${LAGOM_BINARY_DIR}/Userland/Libraries"
    "${LAGOM_BINARY_DIR}/Userland/Services"
)
add_compile_definitions(AK_DONT_REPLACE_STD)

include(GNUInstallDirs)
add_compile_definitions("LOCALEDIR=\"${CMAKE_INSTALL_FULL_LOCALEDIR}\"")

find_package(PkgConfig)

add_subdirectory(res)
add_subdirectory(src)
add_subdirectory(po)

if (NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/InstallRules.cmake)
endif()
