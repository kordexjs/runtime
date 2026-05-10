/**
 *
 *  @file RuntimeLoop.cpp
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
#include <utility>
#include <cstdint>

#include <kordex/runtime/RuntimeLoop.hpp>
#include <vix/runtime/Task.hpp>

namespace kordex::runtime
{
  RuntimeLoop::RuntimeLoop(RuntimeConfig config)
      : config_(std::move(config)),
        state_(),
        cancellation_(),
        diagnostics_(config_.diagnostics),
        async_context_(),
        worker_runtime_(nullptr)
  {
  }

  RuntimeLoop::~RuntimeLoop()
  {
    if (is_running() || state_.is_starting() || state_.is_stopping())
    {
      (void)shutdown();
    }
  }

  Error RuntimeLoop::start()
  {
    const auto validation = config_.validate();
    if (validation)
    {
      state_.mark_failed();
      return validation;
    }

    auto starting = state_.mark_starting();
    if (starting)
    {
      return starting;
    }

    try
    {
      diagnostics_.record(
          DiagnosticLevel::Info,
          "runtime_loop_start",
          "starting Kordex runtime loop");

      if (!worker_runtime_)
      {
        vix::runtime::RuntimeConfig runtime_config{
            static_cast<std::uint32_t>(config_.workers)};

        worker_runtime_ = std::make_unique<vix::runtime::Runtime>(
            runtime_config);
      }

      worker_runtime_->start();

      auto running = state_.mark_running();
      if (running)
      {
        state_.mark_failed();
        return running;
      }

      diagnostics_.record(
          DiagnosticLevel::Info,
          "runtime_loop_running",
          "Kordex runtime loop is running");

      return ok();
    }
    catch (const std::exception &e)
    {
      state_.mark_failed();

      const auto error = make_runtime_error(
          RuntimeErrorCode::InternalError,
          e.what());

      diagnostics_.record_error(
          "runtime_loop_start_failed",
          error);

      return error;
    }
    catch (...)
    {
      state_.mark_failed();

      const auto error = make_runtime_error(
          RuntimeErrorCode::InternalError,
          "unknown error while starting runtime loop");

      diagnostics_.record_error(
          "runtime_loop_start_failed",
          error);

      return error;
    }
  }

  Error RuntimeLoop::stop()
  {
    if (state_.is_stopped())
    {
      return ok();
    }

    if (state_.is_failed())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidState,
          "failed runtime loop cannot be stopped");
    }

    auto stopping = state_.mark_stopping();
    if (stopping)
    {
      if (state_.is_created())
      {
        auto stopped = state_.mark_stopped();
        if (stopped)
        {
          return stopped;
        }

        return ok();
      }

      return stopping;
    }

    try
    {
      diagnostics_.record(
          DiagnosticLevel::Info,
          "runtime_loop_stop",
          "stopping Kordex runtime loop");

      if (worker_runtime_)
      {
        worker_runtime_->stop();
      }

      async_context_.stop();

      auto stopped = state_.mark_stopped();
      if (stopped)
      {
        return stopped;
      }

      diagnostics_.record(
          DiagnosticLevel::Info,
          "runtime_loop_stopped",
          "Kordex runtime loop stopped");

      return ok();
    }
    catch (const std::exception &e)
    {
      state_.mark_failed();

      const auto error = make_runtime_error(
          RuntimeErrorCode::InternalError,
          e.what());

      diagnostics_.record_error(
          "runtime_loop_stop_failed",
          error);

      return error;
    }
    catch (...)
    {
      state_.mark_failed();

      const auto error = make_runtime_error(
          RuntimeErrorCode::InternalError,
          "unknown error while stopping runtime loop");

      diagnostics_.record_error(
          "runtime_loop_stop_failed",
          error);

      return error;
    }
  }

  Error RuntimeLoop::shutdown()
  {
    cancellation_.request_cancel();

    diagnostics_.record(
        DiagnosticLevel::Info,
        "runtime_loop_shutdown",
        "runtime shutdown requested");

    return stop();
  }

  bool RuntimeLoop::is_running() const noexcept
  {
    return state_.is_running();
  }

  RuntimeLifecycleState RuntimeLoop::state() const noexcept
  {
    return state_.value();
  }

  Error RuntimeLoop::post(TaskFunction task)
  {
    if (!task)
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "runtime task cannot be empty");
    }

    if (!is_running() || !worker_runtime_)
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidState,
          "runtime loop is not running");
    }

    try
    {
      vix::runtime::TaskFn runtime_task =
          [task = std::move(task)]() mutable -> vix::runtime::TaskResult
      {
        task();
        return vix::runtime::TaskResult::complete;
      };

      const bool accepted = worker_runtime_->submit(std::move(runtime_task));

      if (!accepted)
      {
        return make_runtime_error(
            RuntimeErrorCode::InvalidState,
            "runtime task was rejected");
      }

      return ok();
    }
    catch (const std::exception &e)
    {
      return make_runtime_error(
          RuntimeErrorCode::InternalError,
          e.what());
    }
    catch (...)
    {
      return make_runtime_error(
          RuntimeErrorCode::InternalError,
          "unknown error while posting runtime task");
    }
  }

  vix::async::core::io_context &RuntimeLoop::io_context() noexcept
  {
    return async_context_;
  }

  const vix::async::core::io_context &RuntimeLoop::io_context() const noexcept
  {
    return async_context_;
  }

  vix::async::core::scheduler &RuntimeLoop::scheduler() noexcept
  {
    return async_context_.get_scheduler();
  }

  CancelToken RuntimeLoop::cancel_token() const noexcept
  {
    return cancellation_.token();
  }

  Cancellation &RuntimeLoop::cancellation() noexcept
  {
    return cancellation_;
  }

  const Cancellation &RuntimeLoop::cancellation() const noexcept
  {
    return cancellation_;
  }

  Diagnostics &RuntimeLoop::diagnostics() noexcept
  {
    return diagnostics_;
  }

  const Diagnostics &RuntimeLoop::diagnostics() const noexcept
  {
    return diagnostics_;
  }

  const RuntimeConfig &RuntimeLoop::config() const noexcept
  {
    return config_;
  }

} // namespace kordex::runtime
