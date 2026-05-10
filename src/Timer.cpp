/**
 *
 *  @file Timer.cpp
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
#include <system_error>
#include <utility>

#include <kordex/runtime/Timer.hpp>

namespace kordex::runtime
{
  bool TimerInfo::waiting() const noexcept
  {
    return status == TimerStatus::Waiting;
  }

  bool TimerInfo::fired() const noexcept
  {
    return status == TimerStatus::Fired;
  }

  bool TimerInfo::cancelled() const noexcept
  {
    return status == TimerStatus::Cancelled;
  }

  bool TimerInfo::failed() const noexcept
  {
    return status == TimerStatus::Failed;
  }

  bool TimerInfo::finished() const noexcept
  {
    return status == TimerStatus::Fired ||
           status == TimerStatus::Cancelled ||
           status == TimerStatus::Failed;
  }

  void TimerInfo::mark_waiting(Duration delay_value) noexcept
  {
    status = TimerStatus::Waiting;
    delay = delay_value;
    started_at = Clock::now();
    finished_at = {};
    error = ok();
  }

  void TimerInfo::mark_fired() noexcept
  {
    status = TimerStatus::Fired;
    error = ok();
    mark_finished_time();
  }

  void TimerInfo::mark_cancelled(Error error_value)
  {
    status = TimerStatus::Cancelled;
    error = std::move(error_value);
    mark_finished_time();
  }

  void TimerInfo::mark_failed(Error error_value)
  {
    status = TimerStatus::Failed;
    error = std::move(error_value);
    mark_finished_time();
  }

  void TimerInfo::mark_finished_time() noexcept
  {
    finished_at = Clock::now();
  }

  Timer::Timer(vix::async::core::io_context &context)
      : context_(context),
        info_()
  {
  }

  vix::async::core::task<Error> Timer::sleep_for(
      Duration delay,
      CancelToken token)
  {
    if (delay.count_ns() < 0)
    {
      auto error = make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "timer delay cannot be negative");

      info_.mark_failed(error);
      co_return error;
    }

    if (token.is_cancelled())
    {
      auto error = make_cancelled_error(
          "timer was cancelled before waiting");

      info_.mark_cancelled(error);
      co_return error;
    }

    info_.mark_waiting(delay);

    try
    {
      co_await context_.timers().sleep_for(
          delay.to_chrono(),
          token);

      if (token.is_cancelled())
      {
        auto error = make_cancelled_error(
            "timer was cancelled");

        info_.mark_cancelled(error);
        co_return error;
      }

      info_.mark_fired();
      co_return ok();
    }
    catch (const std::system_error &e)
    {
      auto error = make_runtime_error(
          RuntimeErrorCode::Cancelled,
          e.what());

      info_.mark_cancelled(error);
      co_return error;
    }
    catch (const std::exception &e)
    {
      auto error = make_runtime_error(
          RuntimeErrorCode::InternalError,
          e.what());

      info_.mark_failed(error);
      co_return error;
    }
    catch (...)
    {
      auto error = make_runtime_error(
          RuntimeErrorCode::InternalError,
          "unknown error while waiting on runtime timer");

      info_.mark_failed(error);
      co_return error;
    }
  }

  vix::async::core::task<Error> Timer::sleep_for(
      std::chrono::milliseconds delay,
      CancelToken token)
  {
    co_return co_await sleep_for(
        Duration(std::chrono::duration_cast<std::chrono::nanoseconds>(delay)),
        token);
  }

  vix::async::core::task<Error> Timer::set_timeout(
      Duration delay,
      TimerCallback callback,
      CancelToken token)
  {
    if (!callback)
    {
      auto error = make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "timer callback cannot be empty");

      info_.mark_failed(error);
      co_return error;
    }

    auto sleep_error = co_await sleep_for(delay, token);
    if (sleep_error)
    {
      co_return sleep_error;
    }

    try
    {
      callback();
      co_return ok();
    }
    catch (const std::exception &e)
    {
      auto error = make_runtime_error(
          RuntimeErrorCode::InternalError,
          e.what());

      info_.mark_failed(error);
      co_return error;
    }
    catch (...)
    {
      auto error = make_runtime_error(
          RuntimeErrorCode::InternalError,
          "unknown error while running timer callback");

      info_.mark_failed(error);
      co_return error;
    }
  }

  const TimerInfo &Timer::info() const noexcept
  {
    return info_;
  }

  vix::async::core::io_context &Timer::context() noexcept
  {
    return context_;
  }

  const char *to_string(
      TimerStatus status) noexcept
  {
    switch (status)
    {
    case TimerStatus::Idle:
      return "idle";
    case TimerStatus::Waiting:
      return "waiting";
    case TimerStatus::Fired:
      return "fired";
    case TimerStatus::Cancelled:
      return "cancelled";
    case TimerStatus::Failed:
      return "failed";
    }

    return "failed";
  }

} // namespace kordex::runtime
