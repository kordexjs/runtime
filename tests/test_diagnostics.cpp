/**
 *
 *  @file test_diagnostics.cpp
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
#include <kordex/runtime/Diagnostics.hpp>

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

  bool test_diagnostic_level_to_string()
  {
    using kordex::runtime::DiagnosticLevel;

    return expect_true(
               std::string_view(kordex::runtime::to_string(DiagnosticLevel::Trace)) == "trace",
               "trace level should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(DiagnosticLevel::Debug)) == "debug",
               "debug level should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(DiagnosticLevel::Info)) == "info",
               "info level should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(DiagnosticLevel::Warning)) == "warning",
               "warning level should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(DiagnosticLevel::Error)) == "error",
               "error level should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(DiagnosticLevel::Critical)) == "critical",
               "critical level should convert to string");
  }

  bool test_diagnostic_level_to_log_level()
  {
    using kordex::runtime::DiagnosticLevel;
    using vix::log::LogLevel;

    return expect_true(
               kordex::runtime::to_log_level(DiagnosticLevel::Trace) == LogLevel::Trace,
               "trace should map to trace log level") &&
           expect_true(
               kordex::runtime::to_log_level(DiagnosticLevel::Debug) == LogLevel::Debug,
               "debug should map to debug log level") &&
           expect_true(
               kordex::runtime::to_log_level(DiagnosticLevel::Info) == LogLevel::Info,
               "info should map to info log level") &&
           expect_true(
               kordex::runtime::to_log_level(DiagnosticLevel::Warning) == LogLevel::Warn,
               "warning should map to warn log level") &&
           expect_true(
               kordex::runtime::to_log_level(DiagnosticLevel::Error) == LogLevel::Error,
               "error should map to error log level") &&
           expect_true(
               kordex::runtime::to_log_level(DiagnosticLevel::Critical) == LogLevel::Critical,
               "critical should map to critical log level");
  }

  bool test_diagnostic_event_create()
  {
    auto event = kordex::runtime::DiagnosticEvent::create(
        kordex::runtime::DiagnosticLevel::Info,
        "runtime_start",
        "runtime started");

    return expect_true(
               event.level == kordex::runtime::DiagnosticLevel::Info,
               "event level should be info") &&
           expect_true(
               event.name == "runtime_start",
               "event name should be preserved") &&
           expect_true(
               event.message == "runtime started",
               "event message should be preserved") &&
           expect_true(
               !event.timestamp.is_zero(),
               "event timestamp should be set") &&
           expect_true(
               !event.has_details(),
               "new event should not have details");
  }

  bool test_diagnostic_event_details()
  {
    auto event = kordex::runtime::DiagnosticEvent::create(
        kordex::runtime::DiagnosticLevel::Debug,
        "source_load",
        "source loaded");

    event.path = "app.js";
    event.details["source"] = "app.js";
    event.details["bytes"] = 42;

    const auto json = event.to_json();

    return expect_true(
               event.has_details(),
               "event should report details") &&
           expect_true(
               json.is_object(),
               "event json should be object") &&
           expect_true(
               json.at("level").get<std::string>() == "debug",
               "event json level should be debug") &&
           expect_true(
               json.at("name").get<std::string>() == "source_load",
               "event json name should be source_load") &&
           expect_true(
               json.at("message").get<std::string>() == "source loaded",
               "event json message should be source loaded") &&
           expect_true(
               json.at("path").get<std::string>() == "app.js",
               "event json path should be app.js") &&
           expect_true(
               json.at("timestamp_ns").get<long long>() > 0,
               "event json timestamp should be positive") &&
           expect_true(
               json.at("details").at("source").get<std::string>() == "app.js",
               "event json details should preserve source") &&
           expect_true(
               json.at("details").at("bytes").get<int>() == 42,
               "event json details should preserve bytes");
  }

  bool test_diagnostics_record_simple_event()
  {
    kordex::runtime::Diagnostics diagnostics;

    diagnostics.record(
        kordex::runtime::DiagnosticLevel::Info,
        "runtime_start",
        "runtime started");

    return expect_true(
               diagnostics.enabled(),
               "diagnostics should be enabled by default") &&
           expect_true(
               diagnostics.size() == 1,
               "diagnostics should contain one event") &&
           expect_true(
               !diagnostics.empty(),
               "diagnostics should not be empty") &&
           expect_true(
               diagnostics.events().front().name == "runtime_start",
               "diagnostics should preserve event name");
  }

  bool test_diagnostics_disable_enable()
  {
    kordex::runtime::Diagnostics diagnostics;

    diagnostics.disable();

    diagnostics.record(
        kordex::runtime::DiagnosticLevel::Info,
        "ignored",
        "this event should be ignored");

    const bool disabled_ok =
        !diagnostics.enabled() &&
        diagnostics.empty();

    diagnostics.enable();

    diagnostics.record(
        kordex::runtime::DiagnosticLevel::Info,
        "recorded",
        "this event should be recorded");

    return expect_true(
               disabled_ok,
               "disabled diagnostics should ignore events") &&
           expect_true(
               diagnostics.enabled(),
               "diagnostics should be enabled again") &&
           expect_true(
               diagnostics.size() == 1,
               "enabled diagnostics should record events") &&
           expect_true(
               diagnostics.events().front().name == "recorded",
               "recorded event should be preserved");
  }

  bool test_diagnostics_record_error()
  {
    kordex::runtime::Diagnostics diagnostics;

    const auto error = kordex::runtime::make_runtime_error(
        kordex::runtime::RuntimeErrorCode::SourceNotFound,
        "source file was not found");

    diagnostics.record_error("runtime_error", error);

    if (diagnostics.size() != 1)
    {
      return false;
    }

    const auto &event = diagnostics.events().front();

    return expect_true(
               event.level == kordex::runtime::DiagnosticLevel::Error,
               "error event should use error level") &&
           expect_true(
               event.name == "runtime_error",
               "error event name should be preserved") &&
           expect_true(
               event.message == "source file was not found",
               "error event message should use error message") &&
           expect_true(
               event.has_details(),
               "error event should have details") &&
           expect_true(
               event.details.at("message").get<std::string>() == "source file was not found",
               "error event details should preserve message");
  }

  bool test_diagnostics_record_result_success()
  {
    kordex::runtime::Diagnostics diagnostics;

    const auto result = kordex::runtime::RuntimeResult::success("ok");

    diagnostics.record_result("runtime_result", result);

    if (diagnostics.size() != 1)
    {
      return false;
    }

    const auto &event = diagnostics.events().front();

    return expect_true(
               event.level == kordex::runtime::DiagnosticLevel::Info,
               "success result should record info event") &&
           expect_true(
               event.name == "runtime_result",
               "result event name should be preserved") &&
           expect_true(
               event.details.at("status").get<std::string>() == "success",
               "result event should preserve status") &&
           expect_true(
               event.details.at("exit_code").get<int>() == 0,
               "result event should preserve exit code") &&
           expect_true(
               event.details.at("has_error").get<bool>() == false,
               "success result should not have error");
  }

  bool test_diagnostics_record_result_failure()
  {
    kordex::runtime::Diagnostics diagnostics;

    const auto error = kordex::runtime::make_runtime_error(
        kordex::runtime::RuntimeErrorCode::InternalError,
        "internal failure");

    const auto result = kordex::runtime::RuntimeResult::failure(error, 1);

    diagnostics.record_result("runtime_result", result);

    if (diagnostics.size() != 1)
    {
      return false;
    }

    const auto &event = diagnostics.events().front();

    return expect_true(
               event.level == kordex::runtime::DiagnosticLevel::Error,
               "failure result should record error event") &&
           expect_true(
               event.details.at("status").get<std::string>() == "failed",
               "failure result should preserve failed status") &&
           expect_true(
               event.details.at("has_error").get<bool>() == true,
               "failure result should report error") &&
           expect_true(
               event.details.at("error").at("message").get<std::string>() == "internal failure",
               "failure result should preserve error message");
  }

  bool test_diagnostics_to_json_and_clear()
  {
    kordex::runtime::Diagnostics diagnostics;

    diagnostics.record(
        kordex::runtime::DiagnosticLevel::Info,
        "runtime_start",
        "runtime started");

    diagnostics.record(
        kordex::runtime::DiagnosticLevel::Warning,
        "runtime_warning",
        "runtime warning");

    const auto json = diagnostics.to_json();

    const bool json_ok =
        json.is_array() &&
        json.size() == 2 &&
        json.at(0).at("name").get<std::string>() == "runtime_start" &&
        json.at(1).at("level").get<std::string>() == "warning";

    diagnostics.clear();

    return expect_true(
               json_ok,
               "diagnostics json should contain recorded events") &&
           expect_true(
               diagnostics.empty(),
               "diagnostics should be empty after clear") &&
           expect_true(
               diagnostics.size() == 0,
               "diagnostics size should be zero after clear");
  }

  bool run_tests()
  {
    return test_diagnostic_level_to_string() &&
           test_diagnostic_level_to_log_level() &&
           test_diagnostic_event_create() &&
           test_diagnostic_event_details() &&
           test_diagnostics_record_simple_event() &&
           test_diagnostics_disable_enable() &&
           test_diagnostics_record_error() &&
           test_diagnostics_record_result_success() &&
           test_diagnostics_record_result_failure() &&
           test_diagnostics_to_json_and_clear();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
