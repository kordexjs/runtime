/**
 *
 *  @file RuntimeState.cpp
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

#include <kordex/runtime/RuntimeState.hpp>

namespace kordex::runtime
{
  RuntimeState::RuntimeState() noexcept
      : state_(RuntimeLifecycleState::Created)
  {
  }

  RuntimeLifecycleState RuntimeState::value() const noexcept
  {
    return state_.load(std::memory_order_acquire);
  }

  bool RuntimeState::is_created() const noexcept
  {
    return value() == RuntimeLifecycleState::Created;
  }

  bool RuntimeState::is_starting() const noexcept
  {
    return value() == RuntimeLifecycleState::Starting;
  }

  bool RuntimeState::is_running() const noexcept
  {
    return value() == RuntimeLifecycleState::Running;
  }

  bool RuntimeState::is_stopping() const noexcept
  {
    return value() == RuntimeLifecycleState::Stopping;
  }

  bool RuntimeState::is_stopped() const noexcept
  {
    return value() == RuntimeLifecycleState::Stopped;
  }

  bool RuntimeState::is_failed() const noexcept
  {
    return value() == RuntimeLifecycleState::Failed;
  }

  bool RuntimeState::can_start() const noexcept
  {
    const auto current = value();

    return current == RuntimeLifecycleState::Created ||
           current == RuntimeLifecycleState::Stopped;
  }

  bool RuntimeState::can_stop() const noexcept
  {
    const auto current = value();

    return current == RuntimeLifecycleState::Starting ||
           current == RuntimeLifecycleState::Running;
  }

  Error RuntimeState::mark_starting() noexcept
  {
    auto expected = RuntimeLifecycleState::Created;

    if (state_.compare_exchange_strong(
            expected,
            RuntimeLifecycleState::Starting,
            std::memory_order_acq_rel))
    {
      return ok();
    }

    expected = RuntimeLifecycleState::Stopped;

    if (state_.compare_exchange_strong(
            expected,
            RuntimeLifecycleState::Starting,
            std::memory_order_acq_rel))
    {
      return ok();
    }

    return make_runtime_error(
        RuntimeErrorCode::InvalidState,
        "runtime cannot start from current state");
  }

  Error RuntimeState::mark_running() noexcept
  {
    auto expected = RuntimeLifecycleState::Starting;

    if (state_.compare_exchange_strong(
            expected,
            RuntimeLifecycleState::Running,
            std::memory_order_acq_rel))
    {
      return ok();
    }

    return make_runtime_error(
        RuntimeErrorCode::InvalidState,
        "runtime cannot become running before starting");
  }

  Error RuntimeState::mark_stopping() noexcept
  {
    auto expected = RuntimeLifecycleState::Running;

    if (state_.compare_exchange_strong(
            expected,
            RuntimeLifecycleState::Stopping,
            std::memory_order_acq_rel))
    {
      return ok();
    }

    expected = RuntimeLifecycleState::Starting;

    if (state_.compare_exchange_strong(
            expected,
            RuntimeLifecycleState::Stopping,
            std::memory_order_acq_rel))
    {
      return ok();
    }

    return make_runtime_error(
        RuntimeErrorCode::InvalidState,
        "runtime cannot stop from current state");
  }

  Error RuntimeState::mark_stopped() noexcept
  {
    if (is_failed())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidState,
          "failed runtime cannot be marked stopped");
    }

    state_.store(RuntimeLifecycleState::Stopped, std::memory_order_release);
    return ok();
  }

  void RuntimeState::mark_failed() noexcept
  {
    state_.store(RuntimeLifecycleState::Failed, std::memory_order_release);
  }

  void RuntimeState::reset() noexcept
  {
    state_.store(RuntimeLifecycleState::Created, std::memory_order_release);
  }

  const char *to_string(RuntimeLifecycleState state) noexcept
  {
    switch (state)
    {
    case RuntimeLifecycleState::Created:
      return "created";
    case RuntimeLifecycleState::Starting:
      return "starting";
    case RuntimeLifecycleState::Running:
      return "running";
    case RuntimeLifecycleState::Stopping:
      return "stopping";
    case RuntimeLifecycleState::Stopped:
      return "stopped";
    case RuntimeLifecycleState::Failed:
      return "failed";
    }

    return "failed";
  }

} // namespace kordex::runtime
