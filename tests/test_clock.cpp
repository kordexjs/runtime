/**
 *
 *  @file test_clock.cpp
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

#include <thread>

#include <kordex/runtime/Clock.hpp>

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

  bool test_wall_clock_timestamp()
  {
    const auto timestamp = kordex::runtime::Clock::now();

    return expect_true(
               !timestamp.is_zero(),
               "runtime timestamp should not be zero") &&
           expect_true(
               timestamp.nanoseconds_since_epoch() > 0,
               "runtime timestamp should be after unix epoch");
  }

  bool test_timestamp_alias()
  {
    const auto timestamp = kordex::runtime::Clock::timestamp();

    return expect_true(
        timestamp.nanoseconds_since_epoch() > 0,
        "timestamp alias should return a valid timestamp");
  }

  bool test_steady_elapsed_time()
  {
    const auto start = kordex::runtime::Clock::steady_now();

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    const auto elapsed = kordex::runtime::Clock::elapsed_since(start);

    return expect_true(
        elapsed.count_ns() >= 0,
        "elapsed duration should be non-negative");
  }

  bool test_duration_helpers()
  {
    const auto one_ms = kordex::runtime::Clock::milliseconds(1);
    const auto one_second = kordex::runtime::Clock::seconds(1);
    const auto one_minute = kordex::runtime::Clock::minutes(1);

    return expect_true(
               one_ms.count_ms() == 1,
               "milliseconds helper should create one millisecond") &&
           expect_true(
               one_second.count_seconds() == 1,
               "seconds helper should create one second") &&
           expect_true(
               one_minute.count_seconds() == 60,
               "minutes helper should create sixty seconds");
  }

  bool run_tests()
  {
    return test_wall_clock_timestamp() &&
           test_timestamp_alias() &&
           test_steady_elapsed_time() &&
           test_duration_helpers();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
