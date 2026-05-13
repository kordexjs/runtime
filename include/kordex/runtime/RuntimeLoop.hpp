/**
 *
 *  @file RuntimeLoop.hpp
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

#ifndef KORDEX_RUNTIME_RUNTIME_LOOP_HPP
#define KORDEX_RUNTIME_RUNTIME_LOOP_HPP

#include <functional>

#include <vix/async/core/io_context.hpp>
#include <vix/async/core/scheduler.hpp>

#include <kordex/runtime/Cancellation.hpp>
#include <kordex/runtime/Diagnostics.hpp>
#include <kordex/runtime/Error.hpp>
#include <kordex/runtime/RuntimeConfig.hpp>
#include <kordex/runtime/RuntimeState.hpp>

namespace kordex::runtime
{
  /**
   * @class RuntimeLoop
   * @brief Runtime execution loop for Kordex.
   *
   * RuntimeLoop owns the internal execution services used by Kordex:
   * - Vix async io_context for async operations
   * - runtime lifecycle state
   * - runtime cancellation controller
   * - runtime diagnostics collector
   *
   * RuntimeLoop is the low-level engine behind the public Runtime facade.
   */
  class RuntimeLoop
  {
  public:
    /**
     * @brief Task function accepted by the runtime loop.
     */
    using TaskFunction = std::function<void()>;

    /**
     * @brief Construct a loop from runtime config.
     */
    explicit RuntimeLoop(RuntimeConfig config = {});

    RuntimeLoop(const RuntimeLoop &) = delete;
    RuntimeLoop &operator=(const RuntimeLoop &) = delete;

    RuntimeLoop(RuntimeLoop &&) noexcept = delete;
    RuntimeLoop &operator=(RuntimeLoop &&) noexcept = delete;

    /**
     * @brief Destroy the loop and stop it if needed.
     */
    ~RuntimeLoop();

    /**
     * @brief Start the runtime loop.
     */
    [[nodiscard]] Error start();

    /**
     * @brief Stop the runtime loop.
     */
    [[nodiscard]] Error stop();

    /**
     * @brief Request shutdown.
     *
     * This requests cancellation and then stops the loop.
     */
    [[nodiscard]] Error shutdown();

    /**
     * @brief Return true if the loop is running.
     */
    [[nodiscard]] bool is_running() const noexcept;

    /**
     * @brief Return current lifecycle state.
     */
    [[nodiscard]] RuntimeLifecycleState state() const noexcept;

    /**
     * @brief Submit a synchronous task to the runtime loop.
     */
    [[nodiscard]] Error post(TaskFunction task);

    /**
     * @brief Return the async io_context.
     */
    [[nodiscard]] vix::async::core::io_context &io_context() noexcept;

    /**
     * @brief Return the async io_context.
     */
    [[nodiscard]] const vix::async::core::io_context &io_context() const noexcept;

    /**
     * @brief Return the async scheduler.
     */
    [[nodiscard]] vix::async::core::scheduler &scheduler() noexcept;

    /**
     * @brief Return the runtime cancellation token.
     */
    [[nodiscard]] CancelToken cancel_token() const noexcept;

    /**
     * @brief Return the cancellation controller.
     */
    [[nodiscard]] Cancellation &cancellation() noexcept;

    /**
     * @brief Return the cancellation controller.
     */
    [[nodiscard]] const Cancellation &cancellation() const noexcept;

    /**
     * @brief Return diagnostics collector.
     */
    [[nodiscard]] Diagnostics &diagnostics() noexcept;

    /**
     * @brief Return diagnostics collector.
     */
    [[nodiscard]] const Diagnostics &diagnostics() const noexcept;

    /**
     * @brief Return runtime config.
     */
    [[nodiscard]] const RuntimeConfig &config() const noexcept;

  private:
    RuntimeConfig config_;
    RuntimeState state_;
    Cancellation cancellation_;
    Diagnostics diagnostics_;
    vix::async::core::io_context async_context_;
  };

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_RUNTIME_LOOP_HPP
