/**
 *
 *  @file Timer.hpp
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

#ifndef KORDEX_RUNTIME_TIMER_HPP
#define KORDEX_RUNTIME_TIMER_HPP

#include <chrono>
#include <functional>

#include <vix/async/core/cancel.hpp>
#include <vix/async/core/io_context.hpp>
#include <vix/async/core/task.hpp>
#include <vix/async/core/timer.hpp>

#include <kordex/runtime/Cancellation.hpp>
#include <kordex/runtime/Clock.hpp>
#include <kordex/runtime/Result.hpp>

namespace kordex::runtime
{
  /**
   * @enum TimerStatus
   * @brief High-level timer lifecycle status.
   */
  enum class TimerStatus
  {
    Idle,
    Waiting,
    Fired,
    Cancelled,
    Failed
  };

  /**
   * @brief Timer callback function.
   */
  using TimerCallback = std::function<void()>;

  /**
   * @struct TimerInfo
   * @brief Runtime timer metadata.
   */
  struct TimerInfo
  {
    /**
     * @brief Current timer status.
     */
    TimerStatus status{TimerStatus::Idle};

    /**
     * @brief Requested delay.
     */
    Duration delay{};

    /**
     * @brief Time when the timer started waiting.
     */
    Timestamp started_at{};

    /**
     * @brief Time when the timer finished.
     */
    Timestamp finished_at{};

    /**
     * @brief Optional timer error.
     */
    Error error{};

    /**
     * @brief Return true if the timer is waiting.
     */
    [[nodiscard]] bool waiting() const noexcept;

    /**
     * @brief Return true if the timer fired successfully.
     */
    [[nodiscard]] bool fired() const noexcept;

    /**
     * @brief Return true if the timer was cancelled.
     */
    [[nodiscard]] bool cancelled() const noexcept;

    /**
     * @brief Return true if the timer failed.
     */
    [[nodiscard]] bool failed() const noexcept;

    /**
     * @brief Return true if the timer finished in any final state.
     */
    [[nodiscard]] bool finished() const noexcept;

    /**
     * @brief Mark the timer as waiting.
     */
    void mark_waiting(Duration delay_value) noexcept;

    /**
     * @brief Mark the timer as fired.
     */
    void mark_fired() noexcept;

    /**
     * @brief Mark the timer as cancelled.
     */
    void mark_cancelled(Error error_value);

    /**
     * @brief Mark the timer as failed.
     */
    void mark_failed(Error error_value);

  private:
    void mark_finished_time() noexcept;
  };

  /**
   * @class Timer
   * @brief Kordex runtime timer facade.
   *
   * Timer wraps Vix async timers and provides a simple Kordex-facing API
   * for sleeping, delaying callbacks, and observing timer state.
   */
  class Timer
  {
  public:
    /**
     * @brief Construct a timer bound to a Vix async context.
     */
    explicit Timer(vix::async::core::io_context &context);

    Timer(const Timer &) = delete;
    Timer &operator=(const Timer &) = delete;

    Timer(Timer &&) noexcept = delete;
    Timer &operator=(Timer &&) noexcept = delete;

    /**
     * @brief Sleep asynchronously for a duration.
     */
    [[nodiscard]] vix::async::core::task<Error> sleep_for(
        Duration delay,
        CancelToken token = {});

    /**
     * @brief Sleep asynchronously for a chrono duration.
     */
    [[nodiscard]] vix::async::core::task<Error> sleep_for(
        std::chrono::milliseconds delay,
        CancelToken token = {});

    /**
     * @brief Run a callback after a delay.
     */
    [[nodiscard]] vix::async::core::task<Error> set_timeout(
        Duration delay,
        TimerCallback callback,
        CancelToken token = {});

    /**
     * @brief Return timer metadata.
     */
    [[nodiscard]] const TimerInfo &info() const noexcept;

    /**
     * @brief Return the bound async context.
     */
    [[nodiscard]] vix::async::core::io_context &context() noexcept;

  private:
    vix::async::core::io_context &context_;
    TimerInfo info_{};
  };

  /**
   * @brief Convert TimerStatus to a stable string.
   */
  [[nodiscard]] const char *to_string(
      TimerStatus status) noexcept;

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_TIMER_HPP
