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

include_guard(GLOBAL)

set(KORDEX_RUNTIME_OPTIONS_INCLUDED ON)

# --------------------------------------------------------------------
# Build options
# --------------------------------------------------------------------
option(KORDEX_RUNTIME_BUILD_TESTS "Build Kordex Runtime tests" OFF)
option(KORDEX_RUNTIME_BUILD_EXAMPLES "Build Kordex Runtime examples" OFF)

# --------------------------------------------------------------------
# Developer options
# --------------------------------------------------------------------
option(KORDEX_RUNTIME_ENABLE_WARNINGS "Enable compiler warnings for Kordex Runtime" ON)
option(KORDEX_RUNTIME_ENABLE_SANITIZERS "Enable sanitizers for Kordex Runtime" OFF)

# --------------------------------------------------------------------
# Dependency version policy
# --------------------------------------------------------------------
set(KORDEX_VIX_GIT_TAG
    "main"
    CACHE STRING
    "Git tag or branch used for Vix dependencies")

# --------------------------------------------------------------------
# Dependency fetch policy
# --------------------------------------------------------------------
option(KORDEX_RUNTIME_FETCH_VIX_DEPS "Auto-fetch missing Vix dependencies" ON)
option(KORDEX_RUNTIME_FETCH_TESTS "Auto-fetch vix::tests if missing" ON)

set(KORDEX_RUNTIME_FETCH_UTILS
    ${KORDEX_RUNTIME_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::utils if missing")

set(KORDEX_RUNTIME_FETCH_ASYNC
    ${KORDEX_RUNTIME_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::async if missing")

set(KORDEX_RUNTIME_FETCH_ERROR
    ${KORDEX_RUNTIME_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::error if missing")

set(KORDEX_RUNTIME_FETCH_LOG
    ${KORDEX_RUNTIME_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::log if missing")

set(KORDEX_RUNTIME_FETCH_FS
    ${KORDEX_RUNTIME_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::fs if missing")

set(KORDEX_RUNTIME_FETCH_PATH
    ${KORDEX_RUNTIME_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::path if missing")

set(KORDEX_RUNTIME_FETCH_ENV
    ${KORDEX_RUNTIME_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::env if missing")

set(KORDEX_RUNTIME_FETCH_PROCESS
    ${KORDEX_RUNTIME_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::process if missing")

set(KORDEX_RUNTIME_FETCH_TIME
    ${KORDEX_RUNTIME_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::time if missing")

set(KORDEX_RUNTIME_FETCH_JSON
    ${KORDEX_RUNTIME_FETCH_VIX_DEPS}
    CACHE BOOL
    "Auto-fetch vix::json if missing")

# --------------------------------------------------------------------
# Umbrella build policy
# --------------------------------------------------------------------
if(DEFINED KORDEX_UMBRELLA_BUILD AND KORDEX_UMBRELLA_BUILD)
  set(KORDEX_RUNTIME_FETCH_VIX_DEPS OFF CACHE BOOL "Auto-fetch missing Vix dependencies" FORCE)
  set(KORDEX_RUNTIME_FETCH_TESTS OFF CACHE BOOL "Auto-fetch vix::tests if missing" FORCE)

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
endif()
