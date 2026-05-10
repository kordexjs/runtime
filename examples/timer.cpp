/**
 *
 *  @file timer.cpp
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

#include <iostream>

#include <vix/async/core/io_context.hpp>
#include <vix/async/core/spawn.hpp>

#include <kordex/runtime/Timer.hpp>

namespace
{
  vix::async::core::task<int> run_timer_example_async(
      vix::async::core::io_context &context)
  {
    kordex::runtime::Timer timer(context);

    std::cout << "timer status: "
              << kordex::runtime::to_string(timer.info().status)
              << '\n';

    auto sleep_error = co_await timer.sleep_for(
        kordex::runtime::Duration::milliseconds(100));

    if (sleep_error)
    {
      std::cerr << "timer sleep failed: "
                << sleep_error.message()
                << '\n';

      co_return 1;
    }

    std::cout << "after sleep status: "
              << kordex::runtime::to_string(timer.info().status)
              << '\n';

    std::cout << "delay ms: "
              << timer.info().delay.count_ms()
              << '\n';

    auto timeout_error = co_await timer.set_timeout(
        kordex::runtime::Duration::milliseconds(100),
        []()
        {
          std::cout << "timeout callback executed\n";
        });

    if (timeout_error)
    {
      std::cerr << "set_timeout failed: "
                << timeout_error.message()
                << '\n';

      co_return 1;
    }

    std::cout << "final timer status: "
              << kordex::runtime::to_string(timer.info().status)
              << '\n';

    co_return 0;
  }

  int run_timer_example()
  {
    vix::async::core::io_context context;

    int exit_code = 1;

    vix::async::core::spawn_detached(
        context,
        [&]() -> vix::async::core::task<void>
        {
          exit_code = co_await run_timer_example_async(context);
          context.stop();
          co_return;
        }());

    context.run();

    return exit_code;
  }
} // namespace

int main()
{
  return run_timer_example();
}
