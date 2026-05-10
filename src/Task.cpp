/**
 *
 *  @file Task.cpp
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

#include <exception>
#include <string>
#include <utility>

#include <kordex/runtime/Task.hpp>

namespace kordex::runtime
{
  bool TaskInfo::succeeded() const noexcept
  {
    return status == TaskStatus::Completed &&
           !error.has_error();
  }

  bool TaskInfo::failed() const noexcept
  {
    return status == TaskStatus::Failed;
  }

  bool TaskInfo::cancelled() const noexcept
  {
    return status == TaskStatus::Cancelled;
  }

  bool TaskInfo::finished() const noexcept
  {
    return status == TaskStatus::Completed ||
           status == TaskStatus::Failed ||
           status == TaskStatus::Cancelled;
  }

  void TaskInfo::mark_running() noexcept
  {
    status = TaskStatus::Running;
    started_at = Clock::now();
    finished_at = {};
    duration = {};
    error = ok();
  }

  void TaskInfo::mark_completed() noexcept
  {
    status = TaskStatus::Completed;
    error = ok();
    mark_finished_time();
  }

  void TaskInfo::mark_failed(Error error_value)
  {
    status = TaskStatus::Failed;
    error = std::move(error_value);
    mark_finished_time();
  }

  void TaskInfo::mark_cancelled(Error error_value)
  {
    status = TaskStatus::Cancelled;
    error = std::move(error_value);
    mark_finished_time();
  }

  void TaskInfo::mark_finished_time() noexcept
  {
    finished_at = Clock::now();

    if (!started_at.is_zero() &&
        finished_at.nanoseconds_since_epoch() >=
            started_at.nanoseconds_since_epoch())
    {
      duration = Duration::nanoseconds(
          finished_at.nanoseconds_since_epoch() -
          started_at.nanoseconds_since_epoch());
    }
  }

  Task::Task(TaskFunction function, TaskOptions options)
      : function_(std::move(function)),
        options_(std::move(options)),
        info_()
  {
    info_.name = options_.name;
  }

  bool Task::valid() const noexcept
  {
    return static_cast<bool>(function_);
  }

  const TaskOptions &Task::options() const noexcept
  {
    return options_;
  }

  const TaskInfo &Task::info() const noexcept
  {
    return info_;
  }

  RuntimeResult Task::run()
  {
    RuntimeResult result;
    result.mark_started();

    if (!function_)
    {
      auto error = make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "runtime task cannot be empty");

      info_.mark_failed(error);

      result = RuntimeResult::failure(error, 1);
      result.mark_finished();
      return result;
    }

    try
    {
      info_.mark_running();

      function_();

      info_.mark_completed();

      result = RuntimeResult::success();
      result.started_at = info_.started_at;
      result.finished_at = info_.finished_at;
      result.duration = info_.duration;

      return result;
    }
    catch (const std::exception &e)
    {
      auto error = make_runtime_error(
          RuntimeErrorCode::InternalError,
          e.what());

      info_.mark_failed(error);

      result = RuntimeResult::failure(error, 1);
      result.started_at = info_.started_at;
      result.finished_at = info_.finished_at;
      result.duration = info_.duration;

      return result;
    }
    catch (...)
    {
      auto error = make_runtime_error(
          RuntimeErrorCode::InternalError,
          "unknown error while running runtime task");

      info_.mark_failed(error);

      result = RuntimeResult::failure(error, 1);
      result.started_at = info_.started_at;
      result.finished_at = info_.finished_at;
      result.duration = info_.duration;

      return result;
    }
  }

  vix::runtime::TaskFn Task::to_vix_task()
  {
    return [this]() -> vix::runtime::TaskResult
    {
      const auto result = run();

      if (result.succeeded())
      {
        return vix::runtime::TaskResult::complete;
      }

      return vix::runtime::TaskResult::failed;
    };
  }

  const char *to_string(
      TaskStatus status) noexcept
  {
    switch (status)
    {
    case TaskStatus::Pending:
      return "pending";
    case TaskStatus::Running:
      return "running";
    case TaskStatus::Completed:
      return "completed";
    case TaskStatus::Failed:
      return "failed";
    case TaskStatus::Cancelled:
      return "cancelled";
    }

    return "failed";
  }

  RuntimeExitStatus to_runtime_exit_status(
      TaskStatus status) noexcept
  {
    switch (status)
    {
    case TaskStatus::Pending:
    case TaskStatus::Running:
      return RuntimeExitStatus::Failed;
    case TaskStatus::Completed:
      return RuntimeExitStatus::Success;
    case TaskStatus::Failed:
      return RuntimeExitStatus::Failed;
    case TaskStatus::Cancelled:
      return RuntimeExitStatus::Cancelled;
    }

    return RuntimeExitStatus::Failed;
  }

} // namespace kordex::runtime
