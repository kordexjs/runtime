/**
 *
 *  @file create_runtime.cpp
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

#include <iostream>
#include <kordex/runtime/Runtime.hpp>

namespace
{
  kordex::runtime::RuntimeConfig make_config()
  {
    kordex::runtime::RuntimeOptions options =
        kordex::runtime::RuntimeOptions::development();

    options.workers = 2;
    options.diagnostics = true;
    options.allow_process = false;

    auto config = kordex::runtime::RuntimeConfig::from_options(options);
    if (!config)
    {
      return {};
    }

    return config.value();
  }

  int run_create_runtime_example()
  {
    kordex::runtime::Runtime runtime(make_config());

    std::cout << "runtime created\n";

    std::cout << "mode: "
              << kordex::runtime::to_string(runtime.config().mode)
              << '\n';

    std::cout << "workers: "
              << runtime.config().workers
              << '\n';

    std::cout << "diagnostics: "
              << (runtime.config().diagnostics ? "enabled" : "disabled")
              << '\n';

    std::cout << "process access: "
              << (runtime.config().allow_process ? "enabled" : "disabled")
              << '\n';

    std::cout << "initial state: "
              << kordex::runtime::to_string(runtime.state())
              << '\n';

    const auto start_error = runtime.start();
    if (start_error)
    {
      std::cerr << "failed to start runtime: "
                << start_error.message()
                << '\n';

      return 1;
    }

    std::cout << "state after start: "
              << kordex::runtime::to_string(runtime.state())
              << '\n';

    runtime.diagnostics().record(
        kordex::runtime::DiagnosticLevel::Info,
        "create_runtime_example",
        "runtime was created and started successfully");

    std::cout << "diagnostic events: "
              << runtime.diagnostics().size()
              << '\n';

    const auto shutdown_error = runtime.shutdown();
    if (shutdown_error)
    {
      std::cerr << "failed to shutdown runtime: "
                << shutdown_error.message()
                << '\n';

      return 1;
    }

    std::cout << "state after shutdown: "
              << kordex::runtime::to_string(runtime.state())
              << '\n';

    return 0;
  }
} // namespace

int main()
{
  return run_create_runtime_example();
}
