/**
 *
 *  @file Clock.cpp
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

#include <kordex/runtime/Clock.hpp>

namespace kordex::runtime
{
  Timestamp Clock::now() noexcept
  {
    return vix::time::SystemClock::now();
  }

  Timestamp Clock::timestamp() noexcept
  {
    return now();
  }

  Clock::steady_time_point Clock::steady_now() noexcept
  {
    return vix::time::SteadyClock::now_chrono();
  }

  Duration Clock::elapsed_since(
      const steady_time_point &start) noexcept
  {
    return vix::time::SteadyClock::since(start);
  }

} // namespace kordex::runtime
