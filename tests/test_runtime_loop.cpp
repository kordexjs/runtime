/**
 *
 *  @file test_runtime_loop.cpp
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
#include <chrono>
#include <thread>

#include <kordex/runtime/RuntimeLoop.hpp>

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

  kordex::runtime::RuntimeConfig make_test_config()
  {
    kordex::runtime::RuntimeOptions options =
        kordex::runtime::RuntimeOptions::test();

    options.workers = 1;
    options.diagnostics = true;
    options.allow_process = true;

    auto config = kordex::runtime::RuntimeConfig::from_options(options);
    if (!config)
    {
      return {};
    }

    return config.value();
  }

  bool wait_until_true(
      const std::atomic<bool> &flag,
      int max_attempts = 50)
  {
    for (int attempt = 0; attempt < max_attempts; ++attempt)
    {
      if (flag.load(std::memory_order_acquire))
      {
        return true;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return false;
  }

  bool test_initial_state()
  {
    kordex::runtime::RuntimeLoop loop(make_test_config());

    return expect_true(
               !loop.is_running(),
               "runtime loop should not be running initially") &&
           expect_true(
               loop.state() == kordex::runtime::RuntimeLifecycleState::Created,
               "runtime loop initial state should be created") &&
           expect_true(
               !loop.cancel_token().is_cancelled(),
               "runtime loop cancellation token should not be cancelled") &&
           expect_true(
               loop.config().workers == 1,
               "runtime loop should preserve config workers") &&
           expect_true(
               loop.diagnostics().enabled(),
               "runtime loop diagnostics should be enabled");
  }

  bool test_start_loop()
  {
    kordex::runtime::RuntimeLoop loop(make_test_config());

    const auto error = loop.start();

    const bool ok =
        !error.has_error() &&
        loop.is_running() &&
        loop.state() == kordex::runtime::RuntimeLifecycleState::Running &&
        loop.diagnostics().size() >= 2;

    (void)loop.shutdown();

    return expect_true(
        ok,
        "runtime loop should start successfully");
  }

  bool test_start_is_rejected_when_already_running()
  {
    kordex::runtime::RuntimeLoop loop(make_test_config());

    const auto first = loop.start();
    const auto second = loop.start();

    const bool ok =
        !first.has_error() &&
        second.has_error() &&
        second.code() == kordex::runtime::ErrorCode::InvalidState &&
        loop.is_running();

    (void)loop.shutdown();

    return expect_true(
        ok,
        "starting an already running loop should fail");
  }

  bool test_stop_loop()
  {
    kordex::runtime::RuntimeLoop loop(make_test_config());

    const auto start_error = loop.start();
    const auto stop_error = loop.stop();

    return expect_true(
               !start_error.has_error(),
               "runtime loop should start before stop") &&
           expect_true(
               !stop_error.has_error(),
               "runtime loop should stop successfully") &&
           expect_true(
               !loop.is_running(),
               "runtime loop should not be running after stop") &&
           expect_true(
               loop.state() == kordex::runtime::RuntimeLifecycleState::Stopped,
               "runtime loop state should be stopped after stop");
  }

  bool test_stop_from_created_is_ok()
  {
    kordex::runtime::RuntimeLoop loop(make_test_config());

    const auto error = loop.stop();

    return expect_true(
               !error.has_error(),
               "stopping a created loop should be ok") &&
           expect_true(
               !loop.is_running(),
               "created loop should remain not running") &&
           expect_true(
               loop.state() == kordex::runtime::RuntimeLifecycleState::Stopped,
               "created loop should become stopped");
  }

  bool test_shutdown_requests_cancellation()
  {
    kordex::runtime::RuntimeLoop loop(make_test_config());

    const auto start_error = loop.start();
    const auto shutdown_error = loop.shutdown();

    return expect_true(
               !start_error.has_error(),
               "runtime loop should start before shutdown") &&
           expect_true(
               !shutdown_error.has_error(),
               "runtime loop should shutdown successfully") &&
           expect_true(
               loop.cancel_token().is_cancelled(),
               "shutdown should request cancellation") &&
           expect_true(
               !loop.is_running(),
               "runtime loop should not be running after shutdown") &&
           expect_true(
               loop.state() == kordex::runtime::RuntimeLifecycleState::Stopped,
               "runtime loop state should be stopped after shutdown");
  }

  bool test_post_rejects_empty_task()
  {
    kordex::runtime::RuntimeLoop loop(make_test_config());

    const auto error = loop.post({});

    return expect_true(
               error.has_error(),
               "empty task should be rejected") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::InvalidArgument,
               "empty task rejection should use InvalidArgument");
  }

  bool test_post_rejects_when_not_running()
  {
    kordex::runtime::RuntimeLoop loop(make_test_config());

    const auto error = loop.post([]() {});

    return expect_true(
               error.has_error(),
               "task should be rejected when loop is not running") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::InvalidState,
               "not running rejection should use InvalidState");
  }

  bool test_post_executes_task()
  {
    kordex::runtime::RuntimeLoop loop(make_test_config());
    std::atomic<bool> executed{false};

    const auto start_error = loop.start();

    const auto post_error = loop.post(
        [&executed]()
        {
          executed.store(true, std::memory_order_release);
        });

    const bool did_execute = wait_until_true(executed);

    const auto shutdown_error = loop.shutdown();

    return expect_true(
               !start_error.has_error(),
               "runtime loop should start") &&
           expect_true(
               !post_error.has_error(),
               "runtime loop should accept task") &&
           expect_true(
               did_execute,
               "runtime loop should execute posted task") &&
           expect_true(
               !shutdown_error.has_error(),
               "runtime loop should shutdown after task");
  }

  bool test_diagnostics_access()
  {
    kordex::runtime::RuntimeLoop loop(make_test_config());

    loop.diagnostics().record(
        kordex::runtime::DiagnosticLevel::Info,
        "manual_event",
        "manual diagnostic event");

    return expect_true(
               loop.diagnostics().size() == 1,
               "runtime loop diagnostics should record manual event") &&
           expect_true(
               loop.diagnostics().events().front().name == "manual_event",
               "runtime loop diagnostics should preserve event name");
  }

  bool test_cancellation_access()
  {
    kordex::runtime::RuntimeLoop loop(make_test_config());

    loop.cancellation().request_cancel();

    return expect_true(
               loop.cancellation().is_cancelled(),
               "runtime loop cancellation controller should be cancelled") &&
           expect_true(
               loop.cancel_token().is_cancelled(),
               "runtime loop cancellation token should observe cancellation");
  }

  bool test_io_context_and_scheduler_access()
  {
    kordex::runtime::RuntimeLoop loop(make_test_config());

    auto &context = loop.io_context();
    auto &scheduler = loop.scheduler();

    (void)context;
    (void)scheduler;

    return expect_true(
        true,
        "runtime loop should expose io_context and scheduler references");
  }

  bool run_tests()
  {
    return test_initial_state() &&
           test_start_loop() &&
           test_start_is_rejected_when_already_running() &&
           test_stop_loop() &&
           test_stop_from_created_is_ok() &&
           test_shutdown_requests_cancellation() &&
           test_post_rejects_empty_task() &&
           test_post_rejects_when_not_running() &&
           test_post_executes_task() &&
           test_diagnostics_access() &&
           test_cancellation_access() &&
           test_io_context_and_scheduler_access();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
