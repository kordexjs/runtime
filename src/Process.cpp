/**
 *
 *  @file Process.cpp
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

#include <string>
#include <utility>

#include <kordex/runtime/Process.hpp>

namespace kordex::runtime
{
  bool ProcessCommand::valid() const noexcept
  {
    return !program.empty();
  }

  vix::process::Command ProcessCommand::to_vix_command() const
  {
    vix::process::Command command(program);

    command.args(args);

    command.stdin_mode(to_vix_pipe_mode(options.stdin_mode));
    command.stdout_mode(to_vix_pipe_mode(options.stdout_mode));
    command.stderr_mode(to_vix_pipe_mode(options.stderr_mode));

    if (!options.working_directory.empty())
    {
      command.cwd(options.working_directory);
    }

    command.search_in_path(options.search_in_path);
    command.inherit_environment(options.inherit_environment);
    command.detach(options.detach);

    return command;
  }

  bool ProcessOutput::success() const noexcept
  {
    return exit_code == 0;
  }

  Process::Process()
      : config_()
  {
  }

  Process::Process(RuntimeConfig config)
      : config_(std::move(config))
  {
  }

  Result<ProcessOutput> Process::output(
      const ProcessCommand &command) const
  {
    const auto permission = check_allowed();
    if (permission)
    {
      return permission;
    }

    if (!command.valid())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "process program cannot be empty");
    }

    auto result = vix::process::output(command.to_vix_command());
    if (!result)
    {
      return make_runtime_error(
          RuntimeErrorCode::ProcessFailed,
          std::string(result.error().message()));
    }

    ProcessOutput output;
    output.exit_code = result.value().exit_code;
    output.stdout_text = result.value().stdout_text;
    output.stderr_text = result.value().stderr_text;

    return output;
  }

  Result<vix::process::Child> Process::spawn(
      const ProcessCommand &command) const
  {
    const auto permission = check_allowed();
    if (permission)
    {
      return permission;
    }

    if (!command.valid())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "process program cannot be empty");
    }

    auto result = vix::process::spawn(command.to_vix_command());
    if (!result)
    {
      return make_runtime_error(
          RuntimeErrorCode::ProcessFailed,
          std::string(result.error().message()));
    }

    return result.value();
  }

  Result<int> Process::wait(
      const vix::process::Child &child) const
  {
    const auto permission = check_allowed();
    if (permission)
    {
      return permission;
    }

    if (!child.valid())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "child process handle is invalid");
    }

    auto result = vix::process::wait(child);
    if (!result)
    {
      return make_runtime_error(
          RuntimeErrorCode::ProcessFailed,
          std::string(result.error().message()));
    }

    return result.value();
  }

  Error Process::terminate(
      const vix::process::Child &child) const
  {
    const auto permission = check_allowed();
    if (permission)
    {
      return permission;
    }

    if (!child.valid())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "child process handle is invalid");
    }

    auto error = vix::process::terminate(child);
    if (error)
    {
      return make_runtime_error(
          RuntimeErrorCode::ProcessFailed,
          std::string(error.message()));
    }

    return ok();
  }

  Error Process::kill(
      const vix::process::Child &child) const
  {
    const auto permission = check_allowed();
    if (permission)
    {
      return permission;
    }

    if (!child.valid())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "child process handle is invalid");
    }

    auto error = vix::process::kill(child);
    if (error)
    {
      return make_runtime_error(
          RuntimeErrorCode::ProcessFailed,
          std::string(error.message()));
    }

    return ok();
  }

  bool Process::allowed() const noexcept
  {
    return config_.allow_process;
  }

  const RuntimeConfig &Process::config() const noexcept
  {
    return config_;
  }

  Error Process::check_allowed() const
  {
    if (!config_.allow_process)
    {
      return make_runtime_error(
          RuntimeErrorCode::PermissionDenied,
          "process access is disabled by runtime config");
    }

    return ok();
  }

  vix::process::PipeMode to_vix_pipe_mode(
      ProcessStreamMode mode) noexcept
  {
    switch (mode)
    {
    case ProcessStreamMode::Inherit:
      return vix::process::PipeMode::Inherit;
    case ProcessStreamMode::Pipe:
      return vix::process::PipeMode::Pipe;
    case ProcessStreamMode::Null:
      return vix::process::PipeMode::Null;
    }

    return vix::process::PipeMode::Inherit;
  }

  const char *to_string(
      ProcessStreamMode mode) noexcept
  {
    switch (mode)
    {
    case ProcessStreamMode::Inherit:
      return "inherit";
    case ProcessStreamMode::Pipe:
      return "pipe";
    case ProcessStreamMode::Null:
      return "null";
    }

    return "inherit";
  }

} // namespace kordex::runtime
