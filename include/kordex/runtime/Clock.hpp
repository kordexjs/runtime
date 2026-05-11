/**
 *
 *  @file Clock.hpp
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

#ifndef KORDEX_RUNTIME_CLOCK_HPP
#define KORDEX_RUNTIME_CLOCK_HPP

#include <vix/time/Clock.hpp>
#include <vix/time/Duration.hpp>
#include <vix/time/Timestamp.hpp>

namespace kordex::runtime
{
  /**
   * @brief Runtime duration type.
   *
   * Duration is used for timeouts, intervals, elapsed time,
   * runtime measurements, and timer APIs.
   */
  using Duration = vix::time::Duration;

  /**
   * @brief Runtime timestamp type.
   *
   * Timestamp is used for diagnostics, runtime events,
   * start/end times, and metadata.
   */
  using Timestamp = vix::time::Timestamp;

  /**
   * @brief Runtime clock facade.
   *
   * Clock provides a stable Kordex-facing wrapper around the Vix time module.
   * It separates:
   * - wall-clock time for timestamps
   * - steady monotonic time for elapsed measurements
   */
  class Clock
  {
  public:
    /**
     * @brief Steady monotonic time point type.
     */
    using steady_time_point = vix::time::SteadyClock::chrono_tp;

    /**
     * @brief Return the current wall-clock timestamp.
     *
     * Use this for runtime events, diagnostics, and metadata.
     */
    [[nodiscard]] static Timestamp now() noexcept;

    /**
     * @brief Return the current wall-clock timestamp.
     *
     * Alias for now(), useful when the call site wants explicit naming.
     */
    [[nodiscard]] static Timestamp timestamp() noexcept;

    /**
     * @brief Return the current monotonic time point.
     *
     * Use this for measuring elapsed time.
     */
    [[nodiscard]] static steady_time_point steady_now() noexcept;

    /**
     * @brief Return elapsed time since a monotonic start point.
     */
    [[nodiscard]] static Duration elapsed_since(
        const steady_time_point &start) noexcept;

    /**
     * @brief Create a duration from milliseconds.
     */
    [[nodiscard]] static constexpr Duration milliseconds(
        Duration::rep value) noexcept
    {
      return Duration::milliseconds(value);
    }

    /**
     * @brief Create a duration from seconds.
     */
    [[nodiscard]] static constexpr Duration seconds(
        Duration::rep value) noexcept
    {
      return Duration::seconds(value);
    }

    /**
     * @brief Create a duration from minutes.
     */
    [[nodiscard]] static constexpr Duration minutes(
        Duration::rep value) noexcept
    {
      return Duration::minutes(value);
    }
  };

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_CLOCK_HPP
