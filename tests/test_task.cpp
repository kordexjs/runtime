/**
 *
 *  @file test_task.cpp
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

#include <atomic>
#include <stdexcept>
#include <string_view>

#include <kordex/runtime/Task.hpp>

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

  bool test_task_status_to_string()
  {
    using kordex::runtime::TaskStatus;

    return expect_true(
               std::string_view(kordex::runtime::to_string(TaskStatus::Pending)) == "pending",
               "pending should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(TaskStatus::Running)) == "running",
               "running should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(TaskStatus::Completed)) == "completed",
               "completed should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(TaskStatus::Failed)) == "failed",
               "failed should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(TaskStatus::Cancelled)) == "cancelled",
               "cancelled should convert to string");
  }

  bool test_task_status_to_runtime_exit_status()
  {
    using kordex::runtime::RuntimeExitStatus;
    using kordex::runtime::TaskStatus;

    return expect_true(
               kordex::runtime::to_runtime_exit_status(TaskStatus::Pending) == RuntimeExitStatus::Failed,
               "pending should map to failed runtime status") &&
           expect_true(
               kordex::runtime::to_runtime_exit_status(TaskStatus::Running) == RuntimeExitStatus::Failed,
               "running should map to failed runtime status") &&
           expect_true(
               kordex::runtime::to_runtime_exit_status(TaskStatus::Completed) == RuntimeExitStatus::Success,
               "completed should map to success runtime status") &&
           expect_true(
               kordex::runtime::to_runtime_exit_status(TaskStatus::Failed) == RuntimeExitStatus::Failed,
               "failed should map to failed runtime status") &&
           expect_true(
               kordex::runtime::to_runtime_exit_status(TaskStatus::Cancelled) == RuntimeExitStatus::Cancelled,
               "cancelled should map to cancelled runtime status");
  }

  bool test_default_task_is_invalid()
  {
    kordex::runtime::Task task;

    return expect_true(
               !task.valid(),
               "default task should be invalid") &&
           expect_true(
               task.info().status == kordex::runtime::TaskStatus::Pending,
               "default task info should be pending");
  }

  bool test_task_options_are_preserved()
  {
    kordex::runtime::TaskOptions options;
    options.name = "demo_task";
    options.critical = true;
    options.may_block = true;

    kordex::runtime::Task task(
        []() {},
        options);

    return expect_true(
               task.valid(),
               "task with function should be valid") &&
           expect_true(
               task.options().name == "demo_task",
               "task should preserve option name") &&
           expect_true(
               task.options().critical,
               "task should preserve critical option") &&
           expect_true(
               task.options().may_block,
               "task should preserve may_block option") &&
           expect_true(
               task.info().name == "demo_task",
               "task info should copy task name");
  }

  bool test_task_info_mark_running()
  {
    kordex::runtime::TaskInfo info;
    info.name = "manual_task";

    info.mark_running();

    return expect_true(
               info.status == kordex::runtime::TaskStatus::Running,
               "mark_running should set running status") &&
           expect_true(
               !info.started_at.is_zero(),
               "mark_running should set started_at") &&
           expect_true(
               info.finished_at.is_zero(),
               "mark_running should clear finished_at") &&
           expect_true(
               info.duration.is_zero(),
               "mark_running should clear duration") &&
           expect_true(
               !info.error.has_error(),
               "mark_running should clear error") &&
           expect_true(
               !info.finished(),
               "running task should not be finished");
  }

  bool test_task_info_mark_completed()
  {
    kordex::runtime::TaskInfo info;

    info.mark_running();
    info.mark_completed();

    return expect_true(
               info.status == kordex::runtime::TaskStatus::Completed,
               "mark_completed should set completed status") &&
           expect_true(
               info.succeeded(),
               "completed task should succeed") &&
           expect_true(
               !info.failed(),
               "completed task should not fail") &&
           expect_true(
               !info.cancelled(),
               "completed task should not be cancelled") &&
           expect_true(
               info.finished(),
               "completed task should be finished") &&
           expect_true(
               !info.finished_at.is_zero(),
               "completed task should set finished_at") &&
           expect_true(
               info.duration.count_ns() >= 0,
               "completed task duration should be non-negative");
  }

  bool test_task_info_mark_failed()
  {
    kordex::runtime::TaskInfo info;

    const auto error = kordex::runtime::make_runtime_error(
        kordex::runtime::RuntimeErrorCode::InternalError,
        "task failed");

    info.mark_running();
    info.mark_failed(error);

    return expect_true(
               info.status == kordex::runtime::TaskStatus::Failed,
               "mark_failed should set failed status") &&
           expect_true(
               !info.succeeded(),
               "failed task should not succeed") &&
           expect_true(
               info.failed(),
               "failed task should report failed") &&
           expect_true(
               !info.cancelled(),
               "failed task should not report cancelled") &&
           expect_true(
               info.finished(),
               "failed task should be finished") &&
           expect_true(
               info.error.has_error(),
               "failed task should preserve error") &&
           expect_true(
               std::string_view(info.error.message()) == "task failed",
               "failed task should preserve error message");
  }

  bool test_task_info_mark_cancelled()
  {
    kordex::runtime::TaskInfo info;

    const auto error = kordex::runtime::make_runtime_error(
        kordex::runtime::RuntimeErrorCode::Cancelled,
        "task cancelled");

    info.mark_running();
    info.mark_cancelled(error);

    return expect_true(
               info.status == kordex::runtime::TaskStatus::Cancelled,
               "mark_cancelled should set cancelled status") &&
           expect_true(
               !info.succeeded(),
               "cancelled task should not succeed") &&
           expect_true(
               !info.failed(),
               "cancelled task should not report failed") &&
           expect_true(
               info.cancelled(),
               "cancelled task should report cancelled") &&
           expect_true(
               info.finished(),
               "cancelled task should be finished") &&
           expect_true(
               info.error.has_error(),
               "cancelled task should preserve error") &&
           expect_true(
               info.error.code() == kordex::runtime::ErrorCode::Cancelled,
               "cancelled task should preserve cancelled error code");
  }

  bool test_run_empty_task_fails()
  {
    kordex::runtime::Task task;

    const auto result = task.run();

    return expect_true(
               !result.succeeded(),
               "empty task run should not succeed") &&
           expect_true(
               result.failed(),
               "empty task run should fail") &&
           expect_true(
               result.error.has_error(),
               "empty task result should contain error") &&
           expect_true(
               result.error.code() == kordex::runtime::ErrorCode::InvalidArgument,
               "empty task result should use InvalidArgument") &&
           expect_true(
               task.info().failed(),
               "empty task info should report failed");
  }

  bool test_run_task_success()
  {
    std::atomic<bool> executed{false};

    kordex::runtime::TaskOptions options;
    options.name = "success_task";

    kordex::runtime::Task task(
        [&executed]()
        {
          executed.store(true, std::memory_order_release);
        },
        options);

    const auto result = task.run();

    return expect_true(
               executed.load(std::memory_order_acquire),
               "task function should execute") &&
           expect_true(
               result.succeeded(),
               "successful task result should succeed") &&
           expect_true(
               result.status == kordex::runtime::RuntimeExitStatus::Success,
               "successful task result should use success status") &&
           expect_true(
               result.exit_code == 0,
               "successful task result should use zero exit code") &&
           expect_true(
               !result.error.has_error(),
               "successful task result should not contain error") &&
           expect_true(
               task.info().succeeded(),
               "task info should report success") &&
           expect_true(
               task.info().status == kordex::runtime::TaskStatus::Completed,
               "task info should be completed") &&
           expect_true(
               !task.info().started_at.is_zero(),
               "task info should set started_at") &&
           expect_true(
               !task.info().finished_at.is_zero(),
               "task info should set finished_at") &&
           expect_true(
               task.info().duration.count_ns() >= 0,
               "task duration should be non-negative");
  }

  bool test_run_task_failure_from_exception()
  {
    kordex::runtime::Task task(
        []()
        {
          throw std::runtime_error("boom");
        });

    const auto result = task.run();

    return expect_true(
               !result.succeeded(),
               "throwing task result should not succeed") &&
           expect_true(
               result.failed(),
               "throwing task result should fail") &&
           expect_true(
               result.status == kordex::runtime::RuntimeExitStatus::Failed,
               "throwing task result should use failed status") &&
           expect_true(
               result.exit_code == 1,
               "throwing task result should use exit code one") &&
           expect_true(
               result.error.has_error(),
               "throwing task result should contain error") &&
           expect_true(
               std::string_view(result.error.message()) == "boom",
               "throwing task result should preserve exception message") &&
           expect_true(
               task.info().failed(),
               "throwing task info should report failed") &&
           expect_true(
               task.info().error.has_error(),
               "throwing task info should preserve error");
  }

  bool test_run_task_failure_from_unknown_exception()
  {
    kordex::runtime::Task task(
        []()
        {
          throw 42;
        });

    const auto result = task.run();

    return expect_true(
               !result.succeeded(),
               "unknown exception task result should not succeed") &&
           expect_true(
               result.failed(),
               "unknown exception task result should fail") &&
           expect_true(
               result.error.has_error(),
               "unknown exception task result should contain error") &&
           expect_true(
               std::string_view(result.error.message()) == "unknown error while running runtime task",
               "unknown exception task should use fallback message") &&
           expect_true(
               task.info().failed(),
               "unknown exception task info should report failed");
  }

  bool test_to_vix_task_success()
  {
    std::atomic<bool> executed{false};

    kordex::runtime::Task task(
        [&executed]()
        {
          executed.store(true, std::memory_order_release);
        });

    auto vix_task = task.to_vix_task();
    const auto result = vix_task();

    return expect_true(
               executed.load(std::memory_order_acquire),
               "Vix task wrapper should execute task") &&
           expect_true(
               result == vix::runtime::TaskResult::complete,
               "successful Vix task wrapper should return complete") &&
           expect_true(
               task.info().succeeded(),
               "successful Vix task wrapper should update task info");
  }

  bool test_to_vix_task_failure()
  {
    kordex::runtime::Task task(
        []()
        {
          throw std::runtime_error("vix task failed");
        });

    auto vix_task = task.to_vix_task();
    const auto result = vix_task();

    return expect_true(
               result == vix::runtime::TaskResult::failed,
               "failed Vix task wrapper should return failed") &&
           expect_true(
               task.info().failed(),
               "failed Vix task wrapper should update task info") &&
           expect_true(
               std::string_view(task.info().error.message()) == "vix task failed",
               "failed Vix task wrapper should preserve error message");
  }

  bool run_tests()
  {
    return test_task_status_to_string() &&
           test_task_status_to_runtime_exit_status() &&
           test_default_task_is_invalid() &&
           test_task_options_are_preserved() &&
           test_task_info_mark_running() &&
           test_task_info_mark_completed() &&
           test_task_info_mark_failed() &&
           test_task_info_mark_cancelled() &&
           test_run_empty_task_fails() &&
           test_run_task_success() &&
           test_run_task_failure_from_exception() &&
           test_run_task_failure_from_unknown_exception() &&
           test_to_vix_task_success() &&
           test_to_vix_task_failure();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
