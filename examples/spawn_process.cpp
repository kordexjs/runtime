/**
 *
 *  @file spawn_process.cpp
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

#include <iostream>

#include <kordex/runtime/Process.hpp>
#include <kordex/runtime/RuntimeConfig.hpp>
#include <kordex/runtime/RuntimeOptions.hpp>

namespace
{
  kordex::runtime::RuntimeConfig make_process_config()
  {
    kordex::runtime::RuntimeOptions options =
        kordex::runtime::RuntimeOptions::development();

    options.allow_process = true;

    auto config = kordex::runtime::RuntimeConfig::from_options(options);
    if (!config)
    {
      return {};
    }

    return config.value();
  }

  kordex::runtime::ProcessCommand make_echo_command()
  {
    kordex::runtime::ProcessCommand command;

#ifndef _WIN32
    command.program = "printf";
    command.args = {"hello from kordex runtime\n"};
#else
    command.program = "cmd";
    command.args = {"/C", "echo hello from kordex runtime"};
#endif

    command.options.stdout_mode = kordex::runtime::ProcessStreamMode::Pipe;
    command.options.stderr_mode = kordex::runtime::ProcessStreamMode::Pipe;
    command.options.stdin_mode = kordex::runtime::ProcessStreamMode::Null;

    return command;
  }

  int run_spawn_process_example()
  {
    kordex::runtime::Process process(make_process_config());

    if (!process.allowed())
    {
      std::cerr << "process access is disabled\n";
      return 1;
    }

    const auto command = make_echo_command();
    const auto output = process.output(command);

    if (!output)
    {
      std::cerr << "process failed: "
                << output.error().message()
                << '\n';

      return 1;
    }

    std::cout << "exit code: "
              << output.value().exit_code
              << '\n';

    if (!output.value().stdout_text.empty())
    {
      std::cout << "stdout: "
                << output.value().stdout_text;
    }

    if (!output.value().stderr_text.empty())
    {
      std::cerr << "stderr: "
                << output.value().stderr_text;
    }

    return output.value().success() ? 0 : output.value().exit_code;
  }
} // namespace

int main()
{
  return run_spawn_process_example();
}
