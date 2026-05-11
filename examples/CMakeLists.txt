#  @file examples/CMakeLists.txt
#  @author Softadastra
#
#  Copyright 2026, Softadastra. All rights reserved.
#  https://github.com/softadastra/kordex-runtime
#  Use of this source code is governed by a MIT license
#  that can be found in the LICENSE file.
#
# ====================================================================
# Kordex Runtime - Examples
# ====================================================================

set(KORDEX_RUNTIME_EXAMPLES
    create_runtime
    load_manifest
    run_file
    spawn_process
    timer)

foreach(example_name IN LISTS KORDEX_RUNTIME_EXAMPLES)
  set(example_source "${CMAKE_CURRENT_SOURCE_DIR}/${example_name}.cpp")

  if(EXISTS "${example_source}")
    add_executable(kordex_runtime_${example_name}
        "${example_source}")

    target_link_libraries(kordex_runtime_${example_name}
        PRIVATE
          kordex::runtime)

    target_compile_features(kordex_runtime_${example_name}
        PRIVATE
          cxx_std_20)
  endif()
endforeach()
