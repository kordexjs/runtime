/**
 *
 *  @file test_timer.cpp
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

#include <atomic>
#include <chrono>
#include <string_view>

#include <vix/async/core/io_context.hpp>
#include <vix/async/core/spawn.hpp>

#include <kordex/runtime/Cancellation.hpp>
#include <kordex/runtime/Timer.hpp>

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

  bool run_async(vix::async::core::task<bool> task)
  {
    vix::async::core::io_context context;

    std::atomic<bool> done{false};
    std::atomic<bool> result{false};

    vix::async::core::spawn(
        context,
        [&]() -> vix::async::core::task<void>
        {
          result.store(co_await task, std::memory_order_release);
          done.store(true, std::memory_order_release);
          context.stop();
          co_return;
        }());

    context.run();

    return done.load(std::memory_order_acquire) &&
           result.load(std::memory_order_acquire);
  }

  bool test_timer_status_to_string()
  {
    using kordex::runtime::TimerStatus;

    return expect_true(
               std::string_view(kordex::runtime::to_string(TimerStatus::Idle)) == "idle",
               "idle should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(TimerStatus::Waiting)) == "waiting",
               "waiting should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(TimerStatus::Fired)) == "fired",
               "fired should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(TimerStatus::Cancelled)) == "cancelled",
               "cancelled should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(TimerStatus::Failed)) == "failed",
               "failed should convert to string");
  }

  bool test_timer_info_initial_state()
  {
    kordex::runtime::TimerInfo info;

    return expect_true(
               info.status == kordex::runtime::TimerStatus::Idle,
               "timer info should start idle") &&
           expect_true(
               !info.waiting(),
               "idle timer should not be waiting") &&
           expect_true(
               !info.fired(),
               "idle timer should not be fired") &&
           expect_true(
               !info.cancelled(),
               "idle timer should not be cancelled") &&
           expect_true(
               !info.failed(),
               "idle timer should not be failed") &&
           expect_true(
               !info.finished(),
               "idle timer should not be finished") &&
           expect_true(
               info.delay.is_zero(),
               "idle timer delay should be zero") &&
           expect_true(
               info.started_at.is_zero(),
               "idle timer started_at should be zero") &&
           expect_true(
               info.finished_at.is_zero(),
               "idle timer finished_at should be zero") &&
           expect_true(
               !info.error.has_error(),
               "idle timer should not have error");
  }

  bool test_timer_info_mark_waiting()
  {
    kordex::runtime::TimerInfo info;

    info.mark_waiting(kordex::runtime::Duration::milliseconds(10));

    return expect_true(
               info.status == kordex::runtime::TimerStatus::Waiting,
               "mark_waiting should set waiting status") &&
           expect_true(
               info.waiting(),
               "timer should report waiting") &&
           expect_true(
               !info.finished(),
               "waiting timer should not be finished") &&
           expect_true(
               info.delay.count_ms() == 10,
               "timer delay should be preserved") &&
           expect_true(
               !info.started_at.is_zero(),
               "mark_waiting should set started_at") &&
           expect_true(
               info.finished_at.is_zero(),
               "mark_waiting should clear finished_at") &&
           expect_true(
               !info.error.has_error(),
               "mark_waiting should clear error");
  }

  bool test_timer_info_mark_fired()
  {
    kordex::runtime::TimerInfo info;

    info.mark_waiting(kordex::runtime::Duration::milliseconds(1));
    info.mark_fired();

    return expect_true(
               info.status == kordex::runtime::TimerStatus::Fired,
               "mark_fired should set fired status") &&
           expect_true(
               info.fired(),
               "timer should report fired") &&
           expect_true(
               info.finished(),
               "fired timer should be finished") &&
           expect_true(
               !info.cancelled(),
               "fired timer should not be cancelled") &&
           expect_true(
               !info.failed(),
               "fired timer should not be failed") &&
           expect_true(
               !info.finished_at.is_zero(),
               "mark_fired should set finished_at") &&
           expect_true(
               !info.error.has_error(),
               "fired timer should not have error");
  }

  bool test_timer_info_mark_cancelled()
  {
    kordex::runtime::TimerInfo info;

    const auto error = kordex::runtime::make_cancelled_error(
        "timer cancelled by test");

    info.mark_waiting(kordex::runtime::Duration::milliseconds(1));
    info.mark_cancelled(error);

    return expect_true(
               info.status == kordex::runtime::TimerStatus::Cancelled,
               "mark_cancelled should set cancelled status") &&
           expect_true(
               info.cancelled(),
               "timer should report cancelled") &&
           expect_true(
               info.finished(),
               "cancelled timer should be finished") &&
           expect_true(
               !info.fired(),
               "cancelled timer should not be fired") &&
           expect_true(
               !info.failed(),
               "cancelled timer should not be failed") &&
           expect_true(
               !info.finished_at.is_zero(),
               "mark_cancelled should set finished_at") &&
           expect_true(
               info.error.has_error(),
               "cancelled timer should preserve error") &&
           expect_true(
               info.error.code() == kordex::runtime::ErrorCode::Cancelled,
               "cancelled timer should preserve cancelled code");
  }

  bool test_timer_info_mark_failed()
  {
    kordex::runtime::TimerInfo info;

    const auto error = kordex::runtime::make_runtime_error(
        kordex::runtime::RuntimeErrorCode::InternalError,
        "timer failed by test");

    info.mark_waiting(kordex::runtime::Duration::milliseconds(1));
    info.mark_failed(error);

    return expect_true(
               info.status == kordex::runtime::TimerStatus::Failed,
               "mark_failed should set failed status") &&
           expect_true(
               info.failed(),
               "timer should report failed") &&
           expect_true(
               info.finished(),
               "failed timer should be finished") &&
           expect_true(
               !info.fired(),
               "failed timer should not be fired") &&
           expect_true(
               !info.cancelled(),
               "failed timer should not be cancelled") &&
           expect_true(
               !info.finished_at.is_zero(),
               "mark_failed should set finished_at") &&
           expect_true(
               info.error.has_error(),
               "failed timer should preserve error") &&
           expect_true(
               std::string_view(info.error.message()) == "timer failed by test",
               "failed timer should preserve error message");
  }

  vix::async::core::task<bool> test_sleep_for_zero_async()
  {
    vix::async::core::io_context context;
    kordex::runtime::Timer timer(context);

    const auto error = co_await timer.sleep_for(
        kordex::runtime::Duration::milliseconds(0));

    co_return expect_true(
        !error.has_error(),
        "zero sleep should succeed") &&
        expect_true(
            timer.info().fired(),
            "zero sleep should fire") &&
        expect_true(
            timer.info().finished(),
            "zero sleep should finish") &&
        expect_true(
            !timer.info().started_at.is_zero(),
            "zero sleep should set started_at") &&
        expect_true(
            !timer.info().finished_at.is_zero(),
            "zero sleep should set finished_at");
  }

  bool test_sleep_for_zero()
  {
    return run_async(test_sleep_for_zero_async());
  }

  vix::async::core::task<bool> test_sleep_for_chrono_duration_async()
  {
    vix::async::core::io_context context;
    kordex::runtime::Timer timer(context);

    const auto error = co_await timer.sleep_for(
        std::chrono::milliseconds(0));

    co_return expect_true(
        !error.has_error(),
        "chrono zero sleep should succeed") &&
        expect_true(
            timer.info().fired(),
            "chrono zero sleep should fire") &&
        expect_true(
            timer.info().delay.count_ms() == 0,
            "chrono zero sleep delay should be zero");
  }

  bool test_sleep_for_chrono_duration()
  {
    return run_async(test_sleep_for_chrono_duration_async());
  }

  vix::async::core::task<bool> test_sleep_for_rejects_negative_delay_async()
  {
    vix::async::core::io_context context;
    kordex::runtime::Timer timer(context);

    const auto error = co_await timer.sleep_for(
        kordex::runtime::Duration::milliseconds(-1));

    co_return expect_true(
        error.has_error(),
        "negative delay should fail") &&
        expect_true(
            error.code() == kordex::runtime::ErrorCode::InvalidArgument,
            "negative delay should use InvalidArgument") &&
        expect_true(
            timer.info().failed(),
            "negative delay should mark timer failed") &&
        expect_true(
            timer.info().error.has_error(),
            "negative delay should preserve timer error");
  }

  bool test_sleep_for_rejects_negative_delay()
  {
    return run_async(test_sleep_for_rejects_negative_delay_async());
  }

  vix::async::core::task<bool> test_sleep_for_cancelled_before_waiting_async()
  {
    vix::async::core::io_context context;
    kordex::runtime::Timer timer(context);
    kordex::runtime::Cancellation cancellation;

    cancellation.request_cancel();

    const auto error = co_await timer.sleep_for(
        kordex::runtime::Duration::milliseconds(1),
        cancellation.token());

    co_return expect_true(
        error.has_error(),
        "cancelled token should fail sleep") &&
        expect_true(
            error.code() == kordex::runtime::ErrorCode::Cancelled,
            "cancelled sleep should use Cancelled code") &&
        expect_true(
            timer.info().cancelled(),
            "cancelled sleep should mark timer cancelled") &&
        expect_true(
            timer.info().finished(),
            "cancelled sleep should finish timer");
  }

  bool test_sleep_for_cancelled_before_waiting()
  {
    return run_async(test_sleep_for_cancelled_before_waiting_async());
  }

  vix::async::core::task<bool> test_set_timeout_runs_callback_async()
  {
    vix::async::core::io_context context;
    kordex::runtime::Timer timer(context);
    std::atomic<bool> called{false};

    const auto error = co_await timer.set_timeout(
        kordex::runtime::Duration::milliseconds(0),
        [&called]()
        {
          called.store(true, std::memory_order_release);
        });

    co_return expect_true(
        !error.has_error(),
        "set_timeout should succeed") &&
        expect_true(
            called.load(std::memory_order_acquire),
            "set_timeout should run callback") &&
        expect_true(
            timer.info().fired(),
            "set_timeout should mark timer fired");
  }

  bool test_set_timeout_runs_callback()
  {
    return run_async(test_set_timeout_runs_callback_async());
  }

  vix::async::core::task<bool> test_set_timeout_rejects_empty_callback_async()
  {
    vix::async::core::io_context context;
    kordex::runtime::Timer timer(context);

    const auto error = co_await timer.set_timeout(
        kordex::runtime::Duration::milliseconds(0),
        {});

    co_return expect_true(
        error.has_error(),
        "empty callback should fail") &&
        expect_true(
            error.code() == kordex::runtime::ErrorCode::InvalidArgument,
            "empty callback should use InvalidArgument") &&
        expect_true(
            timer.info().failed(),
            "empty callback should mark timer failed");
  }

  bool test_set_timeout_rejects_empty_callback()
  {
    return run_async(test_set_timeout_rejects_empty_callback_async());
  }

  vix::async::core::task<bool> test_set_timeout_callback_exception_async()
  {
    vix::async::core::io_context context;
    kordex::runtime::Timer timer(context);

    const auto error = co_await timer.set_timeout(
        kordex::runtime::Duration::milliseconds(0),
        []()
        {
          throw std::runtime_error("timer callback failed");
        });

    co_return expect_true(
        error.has_error(),
        "throwing callback should fail") &&
        expect_true(
            error.code() == kordex::runtime::ErrorCode::InternalError,
            "throwing callback should use InternalError") &&
        expect_true(
            timer.info().failed(),
            "throwing callback should mark timer failed") &&
        expect_true(
            std::string_view(timer.info().error.message()) == "timer callback failed",
            "throwing callback should preserve message");
  }

  bool test_set_timeout_callback_exception()
  {
    return run_async(test_set_timeout_callback_exception_async());
  }

  bool test_timer_context_access()
  {
    vix::async::core::io_context context;
    kordex::runtime::Timer timer(context);

    auto &timer_context = timer.context();

    return expect_true(
        &timer_context == &context,
        "timer should expose bound async context");
  }

  bool run_tests()
  {
    return test_timer_status_to_string() &&
           test_timer_info_initial_state() &&
           test_timer_info_mark_waiting() &&
           test_timer_info_mark_fired() &&
           test_timer_info_mark_cancelled() &&
           test_timer_info_mark_failed() &&
           test_sleep_for_zero() &&
           test_sleep_for_chrono_duration() &&
           test_sleep_for_rejects_negative_delay() &&
           test_sleep_for_cancelled_before_waiting() &&
           test_set_timeout_runs_callback() &&
           test_set_timeout_rejects_empty_callback() &&
           test_set_timeout_callback_exception() &&
           test_timer_context_access();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
