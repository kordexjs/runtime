/**
 *
 *  @file RuntimeResult.cpp
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

#include <utility>
#include <kordex/runtime/RuntimeResult.hpp>

namespace kordex::runtime
{
  RuntimeResult RuntimeResult::success(
      std::string output_value)
  {
    RuntimeResult result;
    result.status = RuntimeExitStatus::Success;
    result.exit_code = 0;
    result.error = ok();
    result.output = std::move(output_value);
    return result;
  }

  RuntimeResult RuntimeResult::failure(
      Error error_value,
      int exit_code_value)
  {
    RuntimeResult result;
    result.status = RuntimeExitStatus::Failed;
    result.exit_code = exit_code_value == 0 ? 1 : exit_code_value;
    result.error = std::move(error_value);
    return result;
  }

  RuntimeResult RuntimeResult::cancelled(
      std::string message)
  {
    RuntimeResult result;
    result.status = RuntimeExitStatus::Cancelled;
    result.exit_code = 130;
    result.error = make_runtime_error(
        RuntimeErrorCode::Cancelled,
        std::move(message));
    return result;
  }

  RuntimeResult RuntimeResult::timeout(
      std::string message)
  {
    RuntimeResult result;
    result.status = RuntimeExitStatus::Timeout;
    result.exit_code = 124;
    result.error = make_runtime_error(
        RuntimeErrorCode::Timeout,
        std::move(message));
    return result;
  }

  void RuntimeResult::mark_started() noexcept
  {
    started_at = Clock::now();
  }

  void RuntimeResult::mark_finished() noexcept
  {
    finished_at = Clock::now();

    if (!started_at.is_zero() &&
        finished_at.nanoseconds_since_epoch() >=
            started_at.nanoseconds_since_epoch())
    {
      const auto elapsed_ns =
          finished_at.nanoseconds_since_epoch() -
          started_at.nanoseconds_since_epoch();

      duration = Duration::nanoseconds(elapsed_ns);
    }
  }

  bool RuntimeResult::succeeded() const noexcept
  {
    return status == RuntimeExitStatus::Success &&
           exit_code == 0 &&
           !error.has_error();
  }

  bool RuntimeResult::failed() const noexcept
  {
    return status == RuntimeExitStatus::Failed;
  }

  bool RuntimeResult::was_cancelled() const noexcept
  {
    return status == RuntimeExitStatus::Cancelled;
  }

  bool RuntimeResult::timed_out() const noexcept
  {
    return status == RuntimeExitStatus::Timeout;
  }

  bool RuntimeResult::has_timing() const noexcept
  {
    return !started_at.is_zero() &&
           !finished_at.is_zero() &&
           !duration.is_zero();
  }

  const char *to_string(RuntimeExitStatus status) noexcept
  {
    switch (status)
    {
    case RuntimeExitStatus::Success:
      return "success";
    case RuntimeExitStatus::Failed:
      return "failed";
    case RuntimeExitStatus::Cancelled:
      return "cancelled";
    case RuntimeExitStatus::Timeout:
      return "timeout";
    }

    return "failed";
  }

} // namespace kordex::runtime
