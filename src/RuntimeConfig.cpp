/**
 *
 *  @file RuntimeConfig.cpp
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

#include <algorithm>
#include <cctype>
#include <string>

#include <vix/env/Env.hpp>
#include <kordex/runtime/RuntimeConfig.hpp>

namespace kordex::runtime
{
  namespace
  {
    [[nodiscard]] std::string to_lower(std::string value)
    {
      std::transform(
          value.begin(),
          value.end(),
          value.begin(),
          [](unsigned char c)
          {
            return static_cast<char>(std::tolower(c));
          });

      return value;
    }

    [[nodiscard]] bool env_bool_or(
        const char *key,
        bool fallback)
    {
      auto value = vix::env::get_bool(key);
      if (!value)
      {
        return fallback;
      }

      return value.value();
    }

    [[nodiscard]] std::size_t env_workers_or(
        const char *key,
        std::size_t fallback)
    {
      auto value = vix::env::get_uint(key);
      if (!value)
      {
        return fallback;
      }

      return static_cast<std::size_t>(value.value());
    }

    [[nodiscard]] Duration env_timeout_or(
        const char *key,
        Duration fallback)
    {
      auto value = vix::env::get_int(key);
      if (!value)
      {
        return fallback;
      }

      if (value.value() <= 0)
      {
        return {};
      }

      return Clock::seconds(value.value());
    }
  } // namespace

  Result<RuntimeMode> parse_runtime_mode(
      const std::string &value)
  {
    const std::string normalized = to_lower(value);

    if (normalized == "development" || normalized == "dev")
    {
      return RuntimeMode::Development;
    }

    if (normalized == "production" || normalized == "prod")
    {
      return RuntimeMode::Production;
    }

    if (normalized == "test" || normalized == "testing")
    {
      return RuntimeMode::Test;
    }

    return make_runtime_error(
        RuntimeErrorCode::InvalidConfig,
        "invalid runtime mode: " + value);
  }

  Result<PermissionMode> parse_permission_mode(
      const std::string &value)
  {
    const std::string normalized = to_lower(value);

    if (normalized == "strict")
    {
      return PermissionMode::Strict;
    }

    if (normalized == "relaxed")
    {
      return PermissionMode::Relaxed;
    }

    return make_runtime_error(
        RuntimeErrorCode::InvalidConfig,
        "invalid permission mode: " + value);
  }

  Result<RuntimeConfig> RuntimeConfig::from_options(
      const RuntimeOptions &options)
  {
    RuntimeConfig config;

    config.mode = options.mode;
    config.permission_mode = options.permission_mode;

    config.workers = options.workers == 0 ? 1 : options.workers;

    config.cache_dir = options.cache_dir;
    config.working_directory = options.working_directory;

    config.environment = to_string(options.mode);

    config.timeout = options.timeout;

    config.allow_fs = options.allow_fs;
    config.allow_net = options.allow_net;
    config.allow_process = options.allow_process;
    config.allow_env = options.allow_env;

    config.diagnostics = options.diagnostics;
    config.debug = options.debug;

    const auto validation = config.validate();
    if (validation)
    {
      return validation;
    }

    return config;
  }

  Result<RuntimeConfig> RuntimeConfig::from_environment(
      const RuntimeOptions &base)
  {
    auto result = from_options(base);
    if (!result)
    {
      return result.error();
    }

    RuntimeConfig config = result.value();

    const std::string mode_value = vix::env::get_or(
        "KORDEX_RUNTIME_MODE",
        to_string(config.mode));

    auto parsed_mode = parse_runtime_mode(mode_value);
    if (!parsed_mode)
    {
      return parsed_mode.error();
    }

    config.mode = parsed_mode.value();
    config.environment = vix::env::get_or(
        "KORDEX_ENV",
        to_string(config.mode));

    const std::string permission_value = vix::env::get_or(
        "KORDEX_PERMISSIONS",
        to_string(config.permission_mode));

    auto parsed_permission = parse_permission_mode(permission_value);
    if (!parsed_permission)
    {
      return parsed_permission.error();
    }

    config.permission_mode = parsed_permission.value();

    config.workers = env_workers_or(
        "KORDEX_RUNTIME_WORKERS",
        config.workers);

    config.cache_dir = vix::env::get_or(
        "KORDEX_CACHE_DIR",
        config.cache_dir);

    config.working_directory = vix::env::get_or(
        "KORDEX_WORKING_DIRECTORY",
        config.working_directory);

    config.timeout = env_timeout_or(
        "KORDEX_RUNTIME_TIMEOUT_SECONDS",
        config.timeout);

    config.allow_fs = env_bool_or(
        "KORDEX_ALLOW_FS",
        config.allow_fs);

    config.allow_net = env_bool_or(
        "KORDEX_ALLOW_NET",
        config.allow_net);

    config.allow_process = env_bool_or(
        "KORDEX_ALLOW_PROCESS",
        config.allow_process);

    config.allow_env = env_bool_or(
        "KORDEX_ALLOW_ENV",
        config.allow_env);

    config.diagnostics = env_bool_or(
        "KORDEX_DIAGNOSTICS",
        config.diagnostics);

    config.debug = env_bool_or(
        "KORDEX_DEBUG",
        config.debug);

    const auto validation = config.validate();
    if (validation)
    {
      return validation;
    }

    return config;
  }

  bool RuntimeConfig::has_timeout() const noexcept
  {
    return !timeout.is_zero();
  }

  bool RuntimeConfig::has_cache_dir() const noexcept
  {
    return !cache_dir.empty();
  }

  bool RuntimeConfig::has_working_directory() const noexcept
  {
    return !working_directory.empty();
  }

  Error RuntimeConfig::validate() const
  {
    if (workers == 0)
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidConfig,
          "runtime workers must be greater than zero");
    }

    if (environment.empty())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidConfig,
          "runtime environment cannot be empty");
    }

    return ok();
  }

} // namespace kordex::runtime
