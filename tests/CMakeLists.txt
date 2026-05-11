#  @file tests/CMakeLists.txt
#  @author Softadastra
#
#  Copyright 2026, Softadastra. All rights reserved.
#  https://github.com/softadastra/kordex-runtime
#  Use of this source code is governed by a MIT license
#  that can be found in the LICENSE file.
#
# ====================================================================
# Kordex Runtime - Tests
# ====================================================================

set(KORDEX_RUNTIME_TESTS
    test_cancellation
    test_clock
    test_diagnostics
    test_error
    test_manifest
    test_module_id
    test_module_resolver
    test_process
    test_runtime
    test_runtime_config
    test_runtime_loop
    test_runtime_options
    test_runtime_result
    test_runtime_state
    test_source_file
    test_task
    test_timer
    test_version)

foreach(test_name IN LISTS KORDEX_RUNTIME_TESTS)
  set(test_source "${CMAKE_CURRENT_SOURCE_DIR}/${test_name}.cpp")

  if(EXISTS "${test_source}")
    add_executable(${test_name}
        "${test_source}")

    target_link_libraries(${test_name}
        PRIVATE
          kordex::runtime
          vix::tests)

    target_compile_features(${test_name}
        PRIVATE
          cxx_std_20)

    add_test(
        NAME ${test_name}
        COMMAND ${test_name})
  endif()
endforeach()
