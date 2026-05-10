/**
 *
 *  @file test_runtime_result.cpp
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
#include <thread>

#include <kordex/runtime/RuntimeResult.hpp>

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

  bool test_success_result()
  {
    const auto result = kordex::runtime::RuntimeResult::success("hello");

    return expect_true(
               result.status == kordex::runtime::RuntimeExitStatus::Success,
               "success result should use success status") &&
           expect_true(
               result.exit_code == 0,
               "success result should use exit code zero") &&
           expect_true(
               !result.error.has_error(),
               "success result should not contain an error") &&
           expect_true(
               result.output == "hello",
               "success result should preserve output") &&
           expect_true(
               result.succeeded(),
               "success result should report succeeded") &&
           expect_true(
               !result.failed(),
               "success result should not report failed");
  }

  bool test_failure_result()
  {
    const auto error = kordex::runtime::make_runtime_error(
        kordex::runtime::RuntimeErrorCode::SourceReadFailed,
        "failed to read source");

    const auto result = kordex::runtime::RuntimeResult::failure(error, 2);

    return expect_true(
               result.status == kordex::runtime::RuntimeExitStatus::Failed,
               "failure result should use failed status") &&
           expect_true(
               result.exit_code == 2,
               "failure result should preserve non-zero exit code") &&
           expect_true(
               result.error.has_error(),
               "failure result should contain an error") &&
           expect_true(
               result.failed(),
               "failure result should report failed") &&
           expect_true(
               !result.succeeded(),
               "failure result should not report succeeded");
  }

  bool test_failure_normalizes_zero_exit_code()
  {
    const auto error = kordex::runtime::make_runtime_error(
        kordex::runtime::RuntimeErrorCode::InternalError,
        "internal error");

    const auto result = kordex::runtime::RuntimeResult::failure(error, 0);

    return expect_true(
        result.exit_code == 1,
        "failure result should normalize zero exit code to one");
  }

  bool test_cancelled_result()
  {
    const auto result = kordex::runtime::RuntimeResult::cancelled();

    return expect_true(
               result.status == kordex::runtime::RuntimeExitStatus::Cancelled,
               "cancelled result should use cancelled status") &&
           expect_true(
               result.exit_code == 130,
               "cancelled result should use exit code 130") &&
           expect_true(
               result.error.has_error(),
               "cancelled result should contain an error") &&
           expect_true(
               result.was_cancelled(),
               "cancelled result should report cancellation");
  }

  bool test_timeout_result()
  {
    const auto result = kordex::runtime::RuntimeResult::timeout();

    return expect_true(
               result.status == kordex::runtime::RuntimeExitStatus::Timeout,
               "timeout result should use timeout status") &&
           expect_true(
               result.exit_code == 124,
               "timeout result should use exit code 124") &&
           expect_true(
               result.error.has_error(),
               "timeout result should contain an error") &&
           expect_true(
               result.timed_out(),
               "timeout result should report timeout");
  }

  bool test_timing()
  {
    auto result = kordex::runtime::RuntimeResult::success();

    result.mark_started();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    result.mark_finished();

    return expect_true(
               !result.started_at.is_zero(),
               "started_at should be set") &&
           expect_true(
               !result.finished_at.is_zero(),
               "finished_at should be set") &&
           expect_true(
               result.duration.count_ns() >= 0,
               "duration should be non-negative") &&
           expect_true(
               result.has_timing(),
               "result should report timing metadata");
  }

  bool test_status_string_conversion()
  {
    using kordex::runtime::RuntimeExitStatus;

    return expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeExitStatus::Success)) == "success",
               "success should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeExitStatus::Failed)) == "failed",
               "failed should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeExitStatus::Cancelled)) == "cancelled",
               "cancelled should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeExitStatus::Timeout)) == "timeout",
               "timeout should convert to string");
  }

  bool run_tests()
  {
    return test_success_result() &&
           test_failure_result() &&
           test_failure_normalizes_zero_exit_code() &&
           test_cancelled_result() &&
           test_timeout_result() &&
           test_timing() &&
           test_status_string_conversion();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
