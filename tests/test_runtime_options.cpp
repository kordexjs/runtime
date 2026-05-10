/**
 *
 *  @file test_runtime_options.cpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/kordexjs/runtime
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the LICENSE file.
 *
 *  Kordex Runtime
 *
 */

#include <string_view>

#include <kordex/runtime/RuntimeOptions.hpp>

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

  bool test_default_options()
  {
    const auto options = kordex::runtime::RuntimeOptions::defaults();

    return expect_true(
               options.mode == kordex::runtime::RuntimeMode::Development,
               "default mode should be development") &&
           expect_true(
               options.permission_mode == kordex::runtime::PermissionMode::Strict,
               "default permission mode should be strict") &&
           expect_true(
               options.workers == 0,
               "default workers should be zero") &&
           expect_true(
               options.allow_fs,
               "default fs access should be allowed") &&
           expect_true(
               !options.allow_net,
               "default net access should be disabled") &&
           expect_true(
               !options.allow_process,
               "default process access should be disabled") &&
           expect_true(
               options.allow_env,
               "default env access should be allowed") &&
           expect_true(
               options.diagnostics,
               "default diagnostics should be enabled") &&
           expect_true(
               !options.debug,
               "default debug mode should be disabled") &&
           expect_true(
               !options.has_timeout(),
               "default timeout should be disabled") &&
           expect_true(
               !options.has_cache_dir(),
               "default cache directory should be empty") &&
           expect_true(
               !options.has_working_directory(),
               "default working directory should be empty");
  }

  bool test_development_options()
  {
    const auto options = kordex::runtime::RuntimeOptions::development();

    return expect_true(
               options.mode == kordex::runtime::RuntimeMode::Development,
               "development mode should be development") &&
           expect_true(
               options.permission_mode == kordex::runtime::PermissionMode::Relaxed,
               "development permission mode should be relaxed") &&
           expect_true(
               options.allow_fs,
               "development fs access should be allowed") &&
           expect_true(
               options.allow_net,
               "development net access should be allowed") &&
           expect_true(
               options.allow_process,
               "development process access should be allowed") &&
           expect_true(
               options.allow_env,
               "development env access should be allowed") &&
           expect_true(
               options.diagnostics,
               "development diagnostics should be enabled") &&
           expect_true(
               options.debug,
               "development debug mode should be enabled");
  }

  bool test_production_options()
  {
    const auto options = kordex::runtime::RuntimeOptions::production();

    return expect_true(
               options.mode == kordex::runtime::RuntimeMode::Production,
               "production mode should be production") &&
           expect_true(
               options.permission_mode == kordex::runtime::PermissionMode::Strict,
               "production permission mode should be strict") &&
           expect_true(
               options.allow_fs,
               "production fs access should be allowed") &&
           expect_true(
               !options.allow_net,
               "production net access should be disabled by default") &&
           expect_true(
               !options.allow_process,
               "production process access should be disabled by default") &&
           expect_true(
               options.allow_env,
               "production env access should be allowed") &&
           expect_true(
               options.diagnostics,
               "production diagnostics should be enabled") &&
           expect_true(
               !options.debug,
               "production debug mode should be disabled");
  }

  bool test_test_options()
  {
    const auto options = kordex::runtime::RuntimeOptions::test();

    return expect_true(
               options.mode == kordex::runtime::RuntimeMode::Test,
               "test mode should be test") &&
           expect_true(
               options.permission_mode == kordex::runtime::PermissionMode::Relaxed,
               "test permission mode should be relaxed") &&
           expect_true(
               options.has_timeout(),
               "test mode should configure a timeout") &&
           expect_true(
               options.timeout.count_seconds() == 30,
               "test timeout should be 30 seconds") &&
           expect_true(
               options.debug,
               "test debug mode should be enabled");
  }

  bool test_option_state_helpers()
  {
    auto options = kordex::runtime::RuntimeOptions::defaults();

    options.timeout = kordex::runtime::Clock::seconds(5);
    options.cache_dir = ".kordex-cache";
    options.working_directory = "/tmp/kordex";

    return expect_true(
               options.has_timeout(),
               "has_timeout should return true") &&
           expect_true(
               options.has_cache_dir(),
               "has_cache_dir should return true") &&
           expect_true(
               options.has_working_directory(),
               "has_working_directory should return true");
  }

  bool test_string_conversions()
  {
    using kordex::runtime::PermissionMode;
    using kordex::runtime::RuntimeMode;

    return expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeMode::Development)) == "development",
               "development should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeMode::Production)) == "production",
               "production should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeMode::Test)) == "test",
               "test should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(PermissionMode::Strict)) == "strict",
               "strict should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(PermissionMode::Relaxed)) == "relaxed",
               "relaxed should convert to string");
  }

  bool run_tests()
  {
    return test_default_options() &&
           test_development_options() &&
           test_production_options() &&
           test_test_options() &&
           test_option_state_helpers() &&
           test_string_conversions();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
