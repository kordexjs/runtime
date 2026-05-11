#  @file CMakeLists.txt
#  @author Softadastra
#
#  Copyright 2026, Softadastra. All rights reserved.
#  https://github.com/softadastra/kordex-runtime
#  Use of this source code is governed by a MIT license
#  that can be found in the LICENSE file.
#
# ====================================================================
# Kordex Runtime
# ====================================================================

cmake_minimum_required(VERSION 3.20)

project(kordex_runtime
    VERSION 0.1.0
    DESCRIPTION "Native runtime engine for Kordex."
    LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/KordexRuntimeOptions.cmake")

# --------------------------------------------------------------------
# Global settings
# --------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --------------------------------------------------------------------
# Export set selection
# --------------------------------------------------------------------
if(DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD)
  set(KORDEX_RUNTIME_EXPORT_SET KordexTargets)
else()
  set(KORDEX_RUNTIME_EXPORT_SET kordex_runtimeTargets)
endif()

# --------------------------------------------------------------------
# Helpers
# --------------------------------------------------------------------
function(kordex_runtime_require_target target_name package_name sibling_dir fetch_option repository tag)
  if(TARGET ${target_name})
    return()
  endif()

  if(DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD)
    message(FATAL_ERROR
        "[runtime] Umbrella build requires target '${target_name}' to be provided by the root project.")
  endif()

  if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../${sibling_dir}/CMakeLists.txt")
    message(STATUS "[runtime] Adding ${target_name} from sibling: ../${sibling_dir}")

    add_subdirectory(
        "${CMAKE_CURRENT_LIST_DIR}/../${sibling_dir}"
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/${sibling_dir}")
  else()
    find_package(${package_name} CONFIG QUIET)
  endif()

  if(TARGET ${target_name})
    return()
  endif()

  if(${fetch_option})
    include(FetchContent)

    message(STATUS "[runtime] Fetching ${target_name}")

    FetchContent_Declare(
        ${package_name}
        GIT_REPOSITORY ${repository}
        GIT_TAG ${tag}
        GIT_SHALLOW TRUE)

    FetchContent_MakeAvailable(${package_name})
  endif()

  if(NOT TARGET ${target_name})
    message(FATAL_ERROR
        "[runtime] Required dependency target '${target_name}' was not found.\n"
        "Install package '${package_name}', provide sibling '../${sibling_dir}', "
        "or enable ${fetch_option}.")
  endif()
endfunction()

# --------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------
kordex_runtime_require_target(
    vix::utils
    vix_utils
    utils
    KORDEX_RUNTIME_FETCH_UTILS
    https://github.com/vixcpp/utils.git
    ${KORDEX_VIX_GIT_TAG})

kordex_runtime_require_target(
    vix::async
    vix_async
    async
    KORDEX_RUNTIME_FETCH_ASYNC
    https://github.com/vixcpp/async.git
    ${KORDEX_VIX_GIT_TAG})

kordex_runtime_require_target(
    vix::error
    vix_error
    error
    KORDEX_RUNTIME_FETCH_ERROR
    https://github.com/vixcpp/error.git
    ${KORDEX_VIX_GIT_TAG})

kordex_runtime_require_target(
    vix::log
    vix_log
    log
    KORDEX_RUNTIME_FETCH_LOG
    https://github.com/vixcpp/log.git
    ${KORDEX_VIX_GIT_TAG})

kordex_runtime_require_target(
    vix::fs
    vix_fs
    fs
    KORDEX_RUNTIME_FETCH_FS
    https://github.com/vixcpp/fs.git
    ${KORDEX_VIX_GIT_TAG})

kordex_runtime_require_target(
    vix::path
    vix_path
    path
    KORDEX_RUNTIME_FETCH_PATH
    https://github.com/vixcpp/path.git
    ${KORDEX_VIX_GIT_TAG})

kordex_runtime_require_target(
    vix::env
    vix_env
    env
    KORDEX_RUNTIME_FETCH_ENV
    https://github.com/vixcpp/env.git
    ${KORDEX_VIX_GIT_TAG})

kordex_runtime_require_target(
    vix::process
    vix_process
    process
    KORDEX_RUNTIME_FETCH_PROCESS
    https://github.com/vixcpp/process.git
    ${KORDEX_VIX_GIT_TAG})

kordex_runtime_require_target(
    vix::time
    vix_time
    time
    KORDEX_RUNTIME_FETCH_TIME
    https://github.com/vixcpp/time.git
    ${KORDEX_VIX_GIT_TAG})

kordex_runtime_require_target(
    vix::json
    vix_json
    json
    KORDEX_RUNTIME_FETCH_JSON
    https://github.com/vixcpp/json.git
    ${KORDEX_VIX_GIT_TAG})

set(KORDEX_RUNTIME_DEPENDENCIES
    vix::utils
    vix::async
    vix::error
    vix::log
    vix::fs
    vix::path
    vix::env
    vix::process
    vix::time
    vix::json)

# --------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------
file(GLOB_RECURSE KORDEX_RUNTIME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

if(NOT KORDEX_RUNTIME_SOURCES)
  message(FATAL_ERROR "[runtime] No source files found in ${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()

# --------------------------------------------------------------------
# Library
# --------------------------------------------------------------------
add_library(kordex_runtime STATIC
    ${KORDEX_RUNTIME_SOURCES})

add_library(kordex::runtime ALIAS kordex_runtime)

target_compile_features(kordex_runtime
    PUBLIC
      cxx_std_20)

target_include_directories(kordex_runtime
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(kordex_runtime
    PUBLIC
      ${KORDEX_RUNTIME_DEPENDENCIES})

target_compile_definitions(kordex_runtime
    PUBLIC
      KORDEX_RUNTIME_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
      KORDEX_RUNTIME_VERSION_MINOR=${PROJECT_VERSION_MINOR}
      KORDEX_RUNTIME_VERSION_PATCH=${PROJECT_VERSION_PATCH})

if(KORDEX_RUNTIME_ENABLE_WARNINGS)
  target_compile_options(kordex_runtime
      PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>)
endif()

if(KORDEX_RUNTIME_ENABLE_SANITIZERS)
  if(MSVC)
    message(WARNING "[runtime] Sanitizers are not enabled for MSVC in this CMake configuration.")
  else()
    target_compile_options(kordex_runtime
        PRIVATE
          -fno-omit-frame-pointer
          -fsanitize=address,undefined)

    target_link_options(kordex_runtime
        PRIVATE
          -fsanitize=address,undefined)
  endif()
endif()

set_target_properties(kordex_runtime PROPERTIES
    OUTPUT_NAME kordex_runtime
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME runtime)

# --------------------------------------------------------------------
# Tests dependency
# --------------------------------------------------------------------
if(KORDEX_RUNTIME_BUILD_TESTS)
  kordex_runtime_require_target(
      vix::tests
      vix_tests
      tests
      KORDEX_RUNTIME_FETCH_TESTS
      https://github.com/vixcpp/tests.git
      ${KORDEX_VIX_GIT_TAG})
endif()

# --------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------
if(KORDEX_RUNTIME_BUILD_TESTS)
  include(CTest)
  enable_testing()

  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
  endif()
endif()

# --------------------------------------------------------------------
# Examples
# --------------------------------------------------------------------
if(KORDEX_RUNTIME_BUILD_EXAMPLES)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
    add_subdirectory(examples)
  endif()
endif()

# --------------------------------------------------------------------
# Install
# --------------------------------------------------------------------
if(NOT (DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD))

  install(
      TARGETS kordex_runtime
      EXPORT kordex_runtimeTargets
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
      INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  install(
      DIRECTORY include/
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h")

  # --------------------------------------------------------------------
  # Standalone package config/export
  # --------------------------------------------------------------------
  configure_package_config_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/KordexRuntimeConfig.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/kordex_runtimeConfig.cmake"
      INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/kordex_runtime")

  write_basic_package_version_file(
      "${CMAKE_CURRENT_BINARY_DIR}/kordex_runtimeConfigVersion.cmake"
      VERSION ${PROJECT_VERSION}
      COMPATIBILITY SameMajorVersion)

  install(
      FILES
        "${CMAKE_CURRENT_BINARY_DIR}/kordex_runtimeConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/kordex_runtimeConfigVersion.cmake"
      DESTINATION
        "${CMAKE_INSTALL_LIBDIR}/cmake/kordex_runtime")

  install(
      EXPORT kordex_runtimeTargets
      FILE kordex_runtimeTargets.cmake
      NAMESPACE kordex::
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/kordex_runtime")

endif()
# --------------------------------------------------------------------
# Summary
# --------------------------------------------------------------------
message(STATUS "")
message(STATUS "Kordex Runtime")
message(STATUS "  version   : ${PROJECT_VERSION}")
message(STATUS "  target    : kordex::runtime")
message(STATUS "  export    : ${KORDEX_RUNTIME_EXPORT_SET}")
message(STATUS "  compiler  : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  build     : ${CMAKE_BUILD_TYPE}")
message(STATUS "  tests     : ${KORDEX_RUNTIME_BUILD_TESTS}")
message(STATUS "  examples  : ${KORDEX_RUNTIME_BUILD_EXAMPLES}")
message(STATUS "  warnings  : ${KORDEX_RUNTIME_ENABLE_WARNINGS}")
message(STATUS "  sanitizers: ${KORDEX_RUNTIME_ENABLE_SANITIZERS}")
message(STATUS "")
