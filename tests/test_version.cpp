/**
 *
 *  @file test_version.cpp
 *  @author Softadastra
 *
 *  Copyright 2026, Softadastra.
 *  All rights reserved.
 *  https://github.com/softadastra/kordex-runtime
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the LICENSE file.
 *
 *  Kordex Runtime
 *
 */

#include <string_view>

#include <kordex/runtime/Version.hpp>

namespace
{
  bool expect_true(bool condition, const char *message)
  {
    if (!condition)
    {
      (void)message;
      return false;
    }

    return true;
  }

  bool test_version_numbers()
  {
    return expect_true(
               kordex::runtime::version_major() == 0,
               "major version should be 0") &&
           expect_true(
               kordex::runtime::version_minor() == 1,
               "minor version should be 1") &&
           expect_true(
               kordex::runtime::version_patch() == 0,
               "patch version should be 0");
  }

  bool test_version_string()
  {
    constexpr std::string_view expected = "0.1.0";

    return expect_true(
               kordex::runtime::version() == expected,
               "version string should be 0.1.0") &&
           expect_true(
               kordex::runtime::KORDEX_RUNTIME_VERSION == expected,
               "version constant should be 0.1.0");
  }

  bool run_tests()
  {
    return test_version_numbers() &&
           test_version_string();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
