#  @file KordexRuntimeOptions.cmake
#  @author Softadastra
#
#  Copyright 2026, Softadastra. All rights reserved.
#  https://github.com/softadastra/kordex-runtime
#  Use of this source code is governed by a MIT license
#  that can be found in the LICENSE file.
#
# ====================================================================
# Kordex Runtime - Build Options
# ====================================================================

# ifndef(KORDEX_RUNTIME_OPTIONS_INCLUDED)
set(KORDEX_RUNTIME_OPTIONS_INCLUDED ON)

# --------------------------------------------------------------------
# Build options
# --------------------------------------------------------------------
option(KORDEX_RUNTIME_BUILD_TESTS "Build Kordex runtime tests" OFF)
option(KORDEX_RUNTIME_BUILD_EXAMPLES "Build Kordex runtime examples" OFF)

# --------------------------------------------------------------------
# Developer options
# --------------------------------------------------------------------
option(KORDEX_RUNTIME_ENABLE_WARNINGS "Enable compiler warnings for Kordex runtime" ON)
option(KORDEX_RUNTIME_ENABLE_SANITIZERS "Enable sanitizers for Kordex runtime" OFF)

# --------------------------------------------------------------------
# Dependency fetch options
# --------------------------------------------------------------------
option(KORDEX_RUNTIME_FETCH_UTILS "Auto-fetch vix::utils if missing" ON)
option(KORDEX_RUNTIME_FETCH_ASYNC "Auto-fetch vix::async if missing" ON)
option(KORDEX_RUNTIME_FETCH_ERROR "Auto-fetch vix::error if missing" ON)
option(KORDEX_RUNTIME_FETCH_LOG "Auto-fetch vix::log if missing" ON)
option(KORDEX_RUNTIME_FETCH_FS "Auto-fetch vix::fs if missing" ON)
option(KORDEX_RUNTIME_FETCH_PATH "Auto-fetch vix::path if missing" ON)
option(KORDEX_RUNTIME_FETCH_ENV "Auto-fetch vix::env if missing" ON)
option(KORDEX_RUNTIME_FETCH_PROCESS "Auto-fetch vix::process if missing" ON)
option(KORDEX_RUNTIME_FETCH_TIME "Auto-fetch vix::time if missing" ON)
option(KORDEX_RUNTIME_FETCH_JSON "Auto-fetch vix::json if missing" ON)
option(KORDEX_RUNTIME_FETCH_TESTS "Auto-fetch vix::tests if missing" ON)

# --------------------------------------------------------------------
# Umbrella build policy
# --------------------------------------------------------------------
# When Kordex runtime is built inside the kordex umbrella repository,
# dependencies should be provided by the umbrella build.
#
# In that mode, runtime must not fetch dependencies by itself.
# The root project is responsible for add_subdirectory order.
# --------------------------------------------------------------------
if(DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD)
  set(KORDEX_RUNTIME_FETCH_UTILS OFF CACHE BOOL "Auto-fetch vix::utils if missing" FORCE)
  set(KORDEX_RUNTIME_FETCH_ASYNC OFF CACHE BOOL "Auto-fetch vix::async if missing" FORCE)
  set(KORDEX_RUNTIME_FETCH_ERROR OFF CACHE BOOL "Auto-fetch vix::error if missing" FORCE)
  set(KORDEX_RUNTIME_FETCH_LOG OFF CACHE BOOL "Auto-fetch vix::log if missing" FORCE)
  set(KORDEX_RUNTIME_FETCH_FS OFF CACHE BOOL "Auto-fetch vix::fs if missing" FORCE)
  set(KORDEX_RUNTIME_FETCH_PATH OFF CACHE BOOL "Auto-fetch vix::path if missing" FORCE)
  set(KORDEX_RUNTIME_FETCH_ENV OFF CACHE BOOL "Auto-fetch vix::env if missing" FORCE)
  set(KORDEX_RUNTIME_FETCH_PROCESS OFF CACHE BOOL "Auto-fetch vix::process if missing" FORCE)
  set(KORDEX_RUNTIME_FETCH_TIME OFF CACHE BOOL "Auto-fetch vix::time if missing" FORCE)
  set(KORDEX_RUNTIME_FETCH_JSON OFF CACHE BOOL "Auto-fetch vix::json if missing" FORCE)
  set(KORDEX_RUNTIME_FETCH_TESTS OFF CACHE BOOL "Auto-fetch vix::tests if missing" FORCE)
endif()

# endif()
