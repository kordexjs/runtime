#  @file CMakeLists.txt
#  @author Gaspard Kirira
#
#  Copyright 2026, Gaspard Kirira. All rights reserved.
#  https://github.com/kordexjs/runtime
#  Use of this source code is governed by a MIT license
#  that can be found in the LICENSE file.
#
# ====================================================================
# Kordex Runtime
# ====================================================================
# Purpose:
#   Native runtime engine for Kordex.
#
#   Provides runtime options, configuration, diagnostics, source loading,
#   module resolution, timers, process integration, and the public Runtime
#   facade used by the Kordex CLI and bindings.
#
# Public Targets:
#   - kordex_runtime   : The actual library target
#   - kordex::runtime  : Namespaced alias for consumers
#
# Public Dependencies:
#   - vix::async
#   - vix::error
#   - vix::log
#   - vix::fs
#   - vix::path
#   - vix::env
#   - vix::process
#   - vix::time
#   - vix::json
#
# Installation/Export:
#   - Umbrella build exports into: KordexTargets
#   - Standalone build exports into: kordex_runtimeTargets
# ====================================================================

cmake_minimum_required(VERSION 3.20)

project(kordex_runtime
  VERSION 0.1.0
  DESCRIPTION "Native runtime engine for Kordex."
  LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/KordexRuntimeOptions.cmake")

# --------------------------------------------------------------------
# Global settings
# --------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
# Sources discovery
# --------------------------------------------------------------------
file(GLOB_RECURSE KORDEX_RUNTIME_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# --------------------------------------------------------------------
# Dependency helpers
# --------------------------------------------------------------------
function(kordex_runtime_require_target target_name package_name sibling_dir fetch_option repo_url git_tag)
  if(DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD)
    if(NOT TARGET ${target_name})
      message(FATAL_ERROR "[runtime] Umbrella build: ${target_name} must be provided before runtime.")
    endif()
    return()
  endif()

  if(TARGET ${target_name})
    return()
  endif()

  if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../${sibling_dir}/CMakeLists.txt")
    message(STATUS "[runtime] Adding ${target_name} from sibling: ../${sibling_dir}")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../${sibling_dir}" "${sibling_dir}")
  else()
    find_package(${package_name} CONFIG QUIET)
  endif()

  if(TARGET ${target_name})
    return()
  endif()

  if(${fetch_option})
    include(FetchContent)
    message(STATUS "[runtime] Fetching ${target_name} via FetchContent")
    FetchContent_Declare(${package_name}
      GIT_REPOSITORY ${repo_url}
      GIT_TAG        ${git_tag}
    )
    FetchContent_MakeAvailable(${package_name})
  endif()

  if(NOT TARGET ${target_name})
    message(FATAL_ERROR
      "${target_name} not found. Enable ${fetch_option}=ON, install ${package_name}, or provide the target before runtime."
    )
  endif()
endfunction()

# --------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------
kordex_runtime_require_target(
  vix::async
  async
  async
  KORDEX_RUNTIME_FETCH_ASYNC
  https://github.com/vixcpp/async.git
  v0.1.0
)

kordex_runtime_require_target(
  vix::error
  vix_error
  error
  KORDEX_RUNTIME_FETCH_ERROR
  https://github.com/vixcpp/error.git
  v0.1.0
)

kordex_runtime_require_target(
  vix::log
  vix_log
  log
  KORDEX_RUNTIME_FETCH_LOG
  https://github.com/vixcpp/log.git
  v0.1.0
)

kordex_runtime_require_target(
  vix::fs
  vix_fs
  fs
  KORDEX_RUNTIME_FETCH_FS
  https://github.com/vixcpp/fs.git
  v0.1.0
)

kordex_runtime_require_target(
  vix::path
  vix_path
  path
  KORDEX_RUNTIME_FETCH_PATH
  https://github.com/vixcpp/path.git
  v0.1.0
)

kordex_runtime_require_target(
  vix::env
  vix_env
  env
  KORDEX_RUNTIME_FETCH_ENV
  https://github.com/vixcpp/env.git
  v0.1.0
)

kordex_runtime_require_target(
  vix::process
  vix_process
  process
  KORDEX_RUNTIME_FETCH_PROCESS
  https://github.com/vixcpp/process.git
  v0.1.0
)

kordex_runtime_require_target(
  vix::time
  vix_time
  time
  KORDEX_RUNTIME_FETCH_TIME
  https://github.com/vixcpp/time.git
  v0.1.0
)

kordex_runtime_require_target(
  vix::json
  vix_json
  json
  KORDEX_RUNTIME_FETCH_JSON
  https://github.com/vixcpp/json.git
  v0.1.0
)

set(KORDEX_RUNTIME_DEPENDENCIES
  vix::async
  vix::error
  vix::log
  vix::fs
  vix::path
  vix::env
  vix::process
  vix::time
  vix::json
)

# --------------------------------------------------------------------
# Library target
# --------------------------------------------------------------------
add_library(kordex_runtime STATIC ${KORDEX_RUNTIME_SOURCES})
add_library(kordex::runtime ALIAS kordex_runtime)

target_compile_features(kordex_runtime PUBLIC cxx_std_20)

target_include_directories(kordex_runtime
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(kordex_runtime
  PUBLIC
    ${KORDEX_RUNTIME_DEPENDENCIES}
)

if(KORDEX_RUNTIME_ENABLE_WARNINGS AND NOT MSVC)
  target_compile_options(kordex_runtime PRIVATE
    -Wall
    -Wextra
    -Wpedantic
  )
endif()

if(KORDEX_RUNTIME_ENABLE_SANITIZERS AND NOT MSVC)
  target_compile_options(kordex_runtime PRIVATE
    -fno-omit-frame-pointer
    -fsanitize=address,undefined
  )

  target_link_options(kordex_runtime PRIVATE
    -fsanitize=address,undefined
  )
endif()

set_target_properties(kordex_runtime PROPERTIES
  OUTPUT_NAME kordex_runtime
  VERSION ${PROJECT_VERSION}
  SOVERSION 0
  EXPORT_NAME runtime
)

install(TARGETS kordex_runtime
  EXPORT ${KORDEX_RUNTIME_EXPORT_SET}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# --------------------------------------------------------------------
# Install headers
# --------------------------------------------------------------------
if(NOT (DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD))
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
      PATTERN "*.hpp"
      PATTERN "*.h"
  )
endif()

# --------------------------------------------------------------------
# Standalone package config/export
# --------------------------------------------------------------------
if(NOT (DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD))

  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/KordexRuntimeConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/kordex_runtimeConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/kordex_runtime"
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/kordex_runtimeConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
  )

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/kordex_runtimeConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/kordex_runtimeConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/kordex_runtime"
  )

  install(EXPORT kordex_runtimeTargets
    FILE kordex_runtimeTargets.cmake
    NAMESPACE kordex::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/kordex_runtime"
  )

endif()

# --------------------------------------------------------------------
# Tests dependency
# --------------------------------------------------------------------
if(KORDEX_RUNTIME_BUILD_TESTS)
  if(DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD)
    if(NOT TARGET vix::tests)
      message(FATAL_ERROR "[runtime] Umbrella build: vix::tests must be provided before runtime tests.")
    endif()
  else()
    if(NOT TARGET vix::tests)
      if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../tests/CMakeLists.txt")
        message(STATUS "[runtime] Adding tests from sibling: ../tests")
        add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../tests" "tests")
      else()
        find_package(vix_tests CONFIG QUIET)
      endif()

      if(NOT TARGET vix::tests)
        if(KORDEX_RUNTIME_FETCH_TESTS)
          include(FetchContent)
          message(STATUS "[runtime] Fetching vix::tests via FetchContent")
          FetchContent_Declare(vix_tests
            GIT_REPOSITORY https://github.com/vixcpp/tests.git
            GIT_TAG        v0.1.0
          )
          FetchContent_MakeAvailable(vix_tests)
        else()
          message(FATAL_ERROR "vix::tests not found.")
        endif()
      endif()
    endif()
  endif()
endif()

# --------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------
if(KORDEX_RUNTIME_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()

# --------------------------------------------------------------------
# Examples
# --------------------------------------------------------------------
if(KORDEX_RUNTIME_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# --------------------------------------------------------------------
# Summary
# --------------------------------------------------------------------
message(STATUS "------------------------------------------------------")
message(STATUS "kordex::runtime configured (${PROJECT_VERSION})")
message(STATUS "Export set: ${KORDEX_RUNTIME_EXPORT_SET}")
message(STATUS "Async target:   vix::async")
message(STATUS "Error target:   vix::error")
message(STATUS "Log target:     vix::log")
message(STATUS "FS target:      vix::fs")
message(STATUS "Path target:    vix::path")
message(STATUS "Env target:     vix::env")
message(STATUS "Process target: vix::process")
message(STATUS "Time target:    vix::time")
message(STATUS "JSON target:    vix::json")
message(STATUS "Include dir: ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
