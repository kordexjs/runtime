/**
 *
 *  @file RuntimeOptions.cpp
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

#include <kordex/runtime/RuntimeOptions.hpp>

namespace kordex::runtime
{
  RuntimeOptions RuntimeOptions::defaults()
  {
    return RuntimeOptions{};
  }

  RuntimeOptions RuntimeOptions::development()
  {
    RuntimeOptions options;
    options.mode = RuntimeMode::Development;
    options.permission_mode = PermissionMode::Relaxed;
    options.allow_fs = true;
    options.allow_net = true;
    options.allow_process = true;
    options.allow_env = true;
    options.diagnostics = true;
    options.debug = true;
    return options;
  }

  RuntimeOptions RuntimeOptions::production()
  {
    RuntimeOptions options;
    options.mode = RuntimeMode::Production;
    options.permission_mode = PermissionMode::Strict;
    options.allow_fs = true;
    options.allow_net = false;
    options.allow_process = false;
    options.allow_env = true;
    options.diagnostics = true;
    options.debug = false;
    return options;
  }

  RuntimeOptions RuntimeOptions::test()
  {
    RuntimeOptions options;
    options.mode = RuntimeMode::Test;
    options.permission_mode = PermissionMode::Relaxed;
    options.allow_fs = true;
    options.allow_net = false;
    options.allow_process = false;
    options.allow_env = true;
    options.diagnostics = true;
    options.debug = true;
    options.timeout = Clock::seconds(30);
    return options;
  }

  bool RuntimeOptions::has_timeout() const noexcept
  {
    return !timeout.is_zero();
  }

  bool RuntimeOptions::has_cache_dir() const noexcept
  {
    return !cache_dir.empty();
  }

  bool RuntimeOptions::has_working_directory() const noexcept
  {
    return !working_directory.empty();
  }

  const char *to_string(RuntimeMode mode) noexcept
  {
    switch (mode)
    {
    case RuntimeMode::Development:
      return "development";
    case RuntimeMode::Production:
      return "production";
    case RuntimeMode::Test:
      return "test";
    }

    return "development";
  }

  const char *to_string(PermissionMode mode) noexcept
  {
    switch (mode)
    {
    case PermissionMode::Strict:
      return "strict";
    case PermissionMode::Relaxed:
      return "relaxed";
    }

    return "strict";
  }

} // namespace kordex::runtime
