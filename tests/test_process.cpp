/**
 *
 *  @file test_process.cpp
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
#include <vix/process/Child.hpp>
#include <kordex/runtime/Process.hpp>

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

  kordex::runtime::RuntimeConfig make_process_enabled_config()
  {
    kordex::runtime::RuntimeOptions options =
        kordex::runtime::RuntimeOptions::test();

    options.allow_process = true;

    auto config = kordex::runtime::RuntimeConfig::from_options(options);
    if (!config)
    {
      return {};
    }

    return config.value();
  }

  kordex::runtime::RuntimeConfig make_process_disabled_config()
  {
    kordex::runtime::RuntimeOptions options =
        kordex::runtime::RuntimeOptions::test();

    options.allow_process = false;

    auto config = kordex::runtime::RuntimeConfig::from_options(options);
    if (!config)
    {
      return {};
    }

    return config.value();
  }

  bool test_process_stream_mode_to_string()
  {
    using kordex::runtime::ProcessStreamMode;

    return expect_true(
               std::string_view(kordex::runtime::to_string(ProcessStreamMode::Inherit)) == "inherit",
               "inherit stream mode should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(ProcessStreamMode::Pipe)) == "pipe",
               "pipe stream mode should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(ProcessStreamMode::Null)) == "null",
               "null stream mode should convert to string");
  }

  bool test_process_stream_mode_to_vix_pipe_mode()
  {
    using kordex::runtime::ProcessStreamMode;
    using vix::process::PipeMode;

    return expect_true(
               kordex::runtime::to_vix_pipe_mode(ProcessStreamMode::Inherit) == PipeMode::Inherit,
               "inherit stream mode should map to Vix inherit") &&
           expect_true(
               kordex::runtime::to_vix_pipe_mode(ProcessStreamMode::Pipe) == PipeMode::Pipe,
               "pipe stream mode should map to Vix pipe") &&
           expect_true(
               kordex::runtime::to_vix_pipe_mode(ProcessStreamMode::Null) == PipeMode::Null,
               "null stream mode should map to Vix null");
  }

  bool test_process_command_validity()
  {
    kordex::runtime::ProcessCommand empty;
    kordex::runtime::ProcessCommand command;
    command.program = "echo";

    return expect_true(
               !empty.valid(),
               "empty process command should be invalid") &&
           expect_true(
               command.valid(),
               "process command with program should be valid");
  }

  bool test_process_command_to_vix_command()
  {
    kordex::runtime::ProcessCommand command;
    command.program = "echo";
    command.args = {"hello", "kordex"};
    command.options.stdin_mode = kordex::runtime::ProcessStreamMode::Null;
    command.options.stdout_mode = kordex::runtime::ProcessStreamMode::Pipe;
    command.options.stderr_mode = kordex::runtime::ProcessStreamMode::Pipe;
    command.options.working_directory = "/tmp";
    command.options.search_in_path = true;
    command.options.inherit_environment = true;
    command.options.detach = false;

    const auto vix_command = command.to_vix_command();

    return expect_true(
               vix_command.valid(),
               "converted Vix command should be valid") &&
           expect_true(
               vix_command.program() == "echo",
               "converted command should preserve program") &&
           expect_true(
               vix_command.args().size() == 2,
               "converted command should preserve argument count") &&
           expect_true(
               vix_command.args().at(0) == "hello",
               "converted command should preserve first argument") &&
           expect_true(
               vix_command.args().at(1) == "kordex",
               "converted command should preserve second argument") &&
           expect_true(
               vix_command.options().stdin_mode == vix::process::PipeMode::Null,
               "converted command should preserve stdin mode") &&
           expect_true(
               vix_command.options().stdout_mode == vix::process::PipeMode::Pipe,
               "converted command should preserve stdout mode") &&
           expect_true(
               vix_command.options().stderr_mode == vix::process::PipeMode::Pipe,
               "converted command should preserve stderr mode") &&
           expect_true(
               vix_command.options().working_directory == "/tmp",
               "converted command should preserve working directory") &&
           expect_true(
               vix_command.options().search_in_path,
               "converted command should preserve search_in_path") &&
           expect_true(
               vix_command.options().inherit_environment,
               "converted command should preserve inherit_environment") &&
           expect_true(
               !vix_command.options().detach,
               "converted command should preserve detach");
  }

  bool test_process_output_success_helper()
  {
    kordex::runtime::ProcessOutput success;
    success.exit_code = 0;

    kordex::runtime::ProcessOutput failure;
    failure.exit_code = 1;

    return expect_true(
               success.success(),
               "zero exit code should report success") &&
           expect_true(
               !failure.success(),
               "non-zero exit code should not report success");
  }

  bool test_process_default_config_disables_process()
  {
    kordex::runtime::Process process;

    return expect_true(
        !process.allowed(),
        "default process facade should not allow process execution");
  }

  bool test_process_enabled_config_allows_process()
  {
    kordex::runtime::Process process(make_process_enabled_config());

    return expect_true(
               process.allowed(),
               "enabled config should allow process execution") &&
           expect_true(
               process.config().allow_process,
               "process facade should preserve config");
  }

  bool test_process_disabled_config_rejects_output()
  {
    kordex::runtime::Process process(make_process_disabled_config());

    kordex::runtime::ProcessCommand command;
    command.program = "echo";
    command.args = {"hello"};

    const auto result = process.output(command);

    return expect_true(
               !result,
               "disabled process facade should reject output") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::PermissionDenied,
               "disabled process output should use PermissionDenied");
  }

  bool test_process_disabled_config_rejects_spawn()
  {
    kordex::runtime::Process process(make_process_disabled_config());

    kordex::runtime::ProcessCommand command;
    command.program = "echo";
    command.args = {"hello"};

    const auto result = process.spawn(command);

    return expect_true(
               !result,
               "disabled process facade should reject spawn") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::PermissionDenied,
               "disabled process spawn should use PermissionDenied");
  }

  bool test_process_disabled_config_rejects_wait()
  {
    kordex::runtime::Process process(make_process_disabled_config());
    vix::process::Child child(1);

    const auto result = process.wait(child);

    return expect_true(
               !result,
               "disabled process facade should reject wait") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::PermissionDenied,
               "disabled process wait should use PermissionDenied");
  }

  bool test_process_disabled_config_rejects_terminate()
  {
    kordex::runtime::Process process(make_process_disabled_config());
    vix::process::Child child(1);

    const auto error = process.terminate(child);

    return expect_true(
               error.has_error(),
               "disabled process facade should reject terminate") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::PermissionDenied,
               "disabled process terminate should use PermissionDenied");
  }

  bool test_process_disabled_config_rejects_kill()
  {
    kordex::runtime::Process process(make_process_disabled_config());
    vix::process::Child child(1);

    const auto error = process.kill(child);

    return expect_true(
               error.has_error(),
               "disabled process facade should reject kill") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::PermissionDenied,
               "disabled process kill should use PermissionDenied");
  }

  bool test_process_enabled_config_rejects_empty_output_command()
  {
    kordex::runtime::Process process(make_process_enabled_config());

    kordex::runtime::ProcessCommand command;

    const auto result = process.output(command);

    return expect_true(
               !result,
               "empty output command should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::InvalidArgument,
               "empty output command should use InvalidArgument");
  }

  bool test_process_enabled_config_rejects_empty_spawn_command()
  {
    kordex::runtime::Process process(make_process_enabled_config());

    kordex::runtime::ProcessCommand command;

    const auto result = process.spawn(command);

    return expect_true(
               !result,
               "empty spawn command should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::InvalidArgument,
               "empty spawn command should use InvalidArgument");
  }

  bool test_process_enabled_config_rejects_invalid_wait_child()
  {
    kordex::runtime::Process process(make_process_enabled_config());
    vix::process::Child child;

    const auto result = process.wait(child);

    return expect_true(
               !result,
               "invalid wait child should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::InvalidArgument,
               "invalid wait child should use InvalidArgument");
  }

  bool test_process_enabled_config_rejects_invalid_terminate_child()
  {
    kordex::runtime::Process process(make_process_enabled_config());
    vix::process::Child child;

    const auto error = process.terminate(child);

    return expect_true(
               error.has_error(),
               "invalid terminate child should fail") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::InvalidArgument,
               "invalid terminate child should use InvalidArgument");
  }

  bool test_process_enabled_config_rejects_invalid_kill_child()
  {
    kordex::runtime::Process process(make_process_enabled_config());
    vix::process::Child child;

    const auto error = process.kill(child);

    return expect_true(
               error.has_error(),
               "invalid kill child should fail") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::InvalidArgument,
               "invalid kill child should use InvalidArgument");
  }

#ifndef _WIN32
  bool test_process_output_echo()
  {
    kordex::runtime::Process process(make_process_enabled_config());

    kordex::runtime::ProcessCommand command;
    command.program = "printf";
    command.args = {"hello-kordex"};
    command.options.stdout_mode = kordex::runtime::ProcessStreamMode::Pipe;
    command.options.stderr_mode = kordex::runtime::ProcessStreamMode::Pipe;

    const auto result = process.output(command);

    if (!result)
    {
      return false;
    }

    return expect_true(
               result.value().success(),
               "printf command should exit successfully") &&
           expect_true(
               result.value().stdout_text == "hello-kordex",
               "printf command should capture stdout") &&
           expect_true(
               result.value().stderr_text.empty(),
               "printf command should not write stderr");
  }
#else
  bool test_process_output_echo()
  {
    kordex::runtime::Process process(make_process_enabled_config());

    kordex::runtime::ProcessCommand command;
    command.program = "cmd";
    command.args = {"/C", "echo hello-kordex"};
    command.options.stdout_mode = kordex::runtime::ProcessStreamMode::Pipe;
    command.options.stderr_mode = kordex::runtime::ProcessStreamMode::Pipe;

    const auto result = process.output(command);

    if (!result)
    {
      return false;
    }

    return expect_true(
               result.value().success(),
               "cmd echo command should exit successfully") &&
           expect_true(
               result.value().stdout_text.find("hello-kordex") != std::string::npos,
               "cmd echo command should capture stdout");
  }
#endif

  bool run_tests()
  {
    return test_process_stream_mode_to_string() &&
           test_process_stream_mode_to_vix_pipe_mode() &&
           test_process_command_validity() &&
           test_process_command_to_vix_command() &&
           test_process_output_success_helper() &&
           test_process_default_config_disables_process() &&
           test_process_enabled_config_allows_process() &&
           test_process_disabled_config_rejects_output() &&
           test_process_disabled_config_rejects_spawn() &&
           test_process_disabled_config_rejects_wait() &&
           test_process_disabled_config_rejects_terminate() &&
           test_process_disabled_config_rejects_kill() &&
           test_process_enabled_config_rejects_empty_output_command() &&
           test_process_enabled_config_rejects_empty_spawn_command() &&
           test_process_enabled_config_rejects_invalid_wait_child() &&
           test_process_enabled_config_rejects_invalid_terminate_child() &&
           test_process_enabled_config_rejects_invalid_kill_child() &&
           test_process_output_echo();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
