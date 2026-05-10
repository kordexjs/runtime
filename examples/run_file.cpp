/**
 *
 *  @file run_file.cpp
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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <kordex/runtime/Runtime.hpp>

namespace
{
  std::filesystem::path example_source_path()
  {
    return std::filesystem::temp_directory_path() /
           "kordex_runtime_run_file_example.js";
  }

  bool write_example_source(const std::filesystem::path &path)
  {
    std::ofstream out(path, std::ios::trunc);
    if (!out)
    {
      return false;
    }

    out << "console.log('hello from Kordex runtime');\n";
    return true;
  }

  void cleanup_example_source(const std::filesystem::path &path)
  {
    std::error_code ec;
    std::filesystem::remove(path, ec);
  }

  kordex::runtime::RuntimeConfig make_config()
  {
    kordex::runtime::RuntimeOptions options =
        kordex::runtime::RuntimeOptions::development();

    options.workers = 1;
    options.diagnostics = true;
    options.allow_process = false;

    auto config = kordex::runtime::RuntimeConfig::from_options(options);
    if (!config)
    {
      return {};
    }

    return config.value();
  }

  int run_file_example()
  {
    const auto path = example_source_path();

    if (!write_example_source(path))
    {
      std::cerr << "failed to create example source file\n";
      return 1;
    }

    kordex::runtime::Runtime runtime(make_config());

    const auto start_error = runtime.start();
    if (start_error)
    {
      cleanup_example_source(path);

      std::cerr << "failed to start runtime: "
                << start_error.message()
                << '\n';

      return 1;
    }

    const auto result = runtime.run_file(path.string());

    const auto shutdown_error = runtime.shutdown();

    cleanup_example_source(path);

    if (shutdown_error)
    {
      std::cerr << "failed to shutdown runtime: "
                << shutdown_error.message()
                << '\n';

      return 1;
    }

    if (!result.succeeded())
    {
      std::cerr << "run_file failed: "
                << result.error.message()
                << '\n';

      return result.exit_code == 0 ? 1 : result.exit_code;
    }

    std::cout << "run_file succeeded\n";

    std::cout << "exit code: "
              << result.exit_code
              << '\n';

    std::cout << "status: "
              << kordex::runtime::to_string(result.status)
              << '\n';

    std::cout << "diagnostic events: "
              << runtime.diagnostics().size()
              << '\n';

    return 0;
  }
} // namespace

int main()
{
  return run_file_example();
}
