/**
 *
 *  @file test_runtime_config.cpp
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
#include <vix/env/Unset.hpp>
#include <kordex/runtime/RuntimeConfig.hpp>

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

  void clear_runtime_env()
  {
    (void)vix::env::unset("KORDEX_RUNTIME_MODE");
    (void)vix::env::unset("KORDEX_ENV");
    (void)vix::env::unset("KORDEX_PERMISSIONS");
    (void)vix::env::unset("KORDEX_RUNTIME_WORKERS");
    (void)vix::env::unset("KORDEX_CACHE_DIR");
    (void)vix::env::unset("KORDEX_WORKING_DIRECTORY");
    (void)vix::env::unset("KORDEX_RUNTIME_TIMEOUT_SECONDS");
    (void)vix::env::unset("KORDEX_ALLOW_FS");
    (void)vix::env::unset("KORDEX_ALLOW_NET");
    (void)vix::env::unset("KORDEX_ALLOW_PROCESS");
    (void)vix::env::unset("KORDEX_ALLOW_ENV");
    (void)vix::env::unset("KORDEX_DIAGNOSTICS");
    (void)vix::env::unset("KORDEX_DEBUG");
  }

  bool test_parse_runtime_mode()
  {
    using kordex::runtime::RuntimeMode;

    auto dev = kordex::runtime::parse_runtime_mode("development");
    auto prod = kordex::runtime::parse_runtime_mode("prod");
    auto test = kordex::runtime::parse_runtime_mode("testing");
    auto invalid = kordex::runtime::parse_runtime_mode("bad");

    return expect_true(
               dev && dev.value() == RuntimeMode::Development,
               "development should parse") &&
           expect_true(
               prod && prod.value() == RuntimeMode::Production,
               "prod should parse") &&
           expect_true(
               test && test.value() == RuntimeMode::Test,
               "testing should parse") &&
           expect_true(
               !invalid,
               "invalid runtime mode should fail");
  }

  bool test_parse_permission_mode()
  {
    using kordex::runtime::PermissionMode;

    auto strict = kordex::runtime::parse_permission_mode("strict");
    auto relaxed = kordex::runtime::parse_permission_mode("relaxed");
    auto invalid = kordex::runtime::parse_permission_mode("open");

    return expect_true(
               strict && strict.value() == PermissionMode::Strict,
               "strict should parse") &&
           expect_true(
               relaxed && relaxed.value() == PermissionMode::Relaxed,
               "relaxed should parse") &&
           expect_true(
               !invalid,
               "invalid permission mode should fail");
  }

  bool test_from_options_defaults()
  {
    clear_runtime_env();

    auto result = kordex::runtime::RuntimeConfig::from_options(
        kordex::runtime::RuntimeOptions::defaults());

    if (!result)
    {
      return false;
    }

    const auto &config = result.value();

    return expect_true(
               config.mode == kordex::runtime::RuntimeMode::Development,
               "config mode should be development") &&
           expect_true(
               config.permission_mode == kordex::runtime::PermissionMode::Strict,
               "config permission mode should be strict") &&
           expect_true(
               config.workers == 1,
               "workers should be normalized to one") &&
           expect_true(
               std::string_view(config.environment) == "development",
               "environment should be development") &&
           expect_true(
               config.allow_fs,
               "fs should be allowed") &&
           expect_true(
               !config.allow_net,
               "net should be disabled") &&
           expect_true(
               !config.allow_process,
               "process should be disabled") &&
           expect_true(
               config.allow_env,
               "env should be allowed") &&
           expect_true(
               !config.has_timeout(),
               "timeout should be disabled");
  }

  bool test_from_options_custom()
  {
    kordex::runtime::RuntimeOptions options;
    options.mode = kordex::runtime::RuntimeMode::Production;
    options.permission_mode = kordex::runtime::PermissionMode::Strict;
    options.workers = 4;
    options.cache_dir = ".kordex";
    options.working_directory = "/tmp";
    options.timeout = kordex::runtime::Clock::seconds(10);
    options.allow_process = true;

    auto result = kordex::runtime::RuntimeConfig::from_options(options);
    if (!result)
    {
      return false;
    }

    const auto &config = result.value();

    return expect_true(
               config.mode == kordex::runtime::RuntimeMode::Production,
               "custom config mode should be production") &&
           expect_true(
               config.workers == 4,
               "custom workers should be preserved") &&
           expect_true(
               config.has_cache_dir(),
               "custom cache directory should be present") &&
           expect_true(
               config.has_working_directory(),
               "custom working directory should be present") &&
           expect_true(
               config.has_timeout(),
               "custom timeout should be present") &&
           expect_true(
               config.allow_process,
               "custom process access should be preserved");
  }

  bool test_validate_invalid_workers()
  {
    kordex::runtime::RuntimeConfig config;
    config.workers = 0;

    const auto error = config.validate();

    return expect_true(
        error.has_error(),
        "zero workers should be invalid");
  }

  bool run_tests()
  {
    clear_runtime_env();

    return test_parse_runtime_mode() &&
           test_parse_permission_mode() &&
           test_from_options_defaults() &&
           test_from_options_custom() &&
           test_validate_invalid_workers();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
