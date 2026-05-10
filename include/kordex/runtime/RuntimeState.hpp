/**
 *
 *  @file RuntimeState.hpp
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

#ifndef KORDEX_RUNTIME_RUNTIME_STATE_HPP
#define KORDEX_RUNTIME_RUNTIME_STATE_HPP

#include <atomic>

#include <kordex/runtime/Error.hpp>

namespace kordex::runtime
{
  /**
   * @enum RuntimeLifecycleState
   * @brief Lifecycle state of a Kordex runtime instance.
   */
  enum class RuntimeLifecycleState
  {
    Created,
    Starting,
    Running,
    Stopping,
    Stopped,
    Failed
  };

  /**
   * @class RuntimeState
   * @brief Thread-safe runtime lifecycle state container.
   *
   * RuntimeState owns the current lifecycle state of a runtime instance.
   * It provides explicit transition helpers used by RuntimeLoop and Runtime.
   */
  class RuntimeState
  {
  public:
    /**
     * @brief Construct a runtime state in Created state.
     */
    RuntimeState() noexcept;

    /**
     * @brief Return the current runtime lifecycle state.
     */
    [[nodiscard]] RuntimeLifecycleState value() const noexcept;

    /**
     * @brief Return true if the runtime is in Created state.
     */
    [[nodiscard]] bool is_created() const noexcept;

    /**
     * @brief Return true if the runtime is starting.
     */
    [[nodiscard]] bool is_starting() const noexcept;

    /**
     * @brief Return true if the runtime is running.
     */
    [[nodiscard]] bool is_running() const noexcept;

    /**
     * @brief Return true if the runtime is stopping.
     */
    [[nodiscard]] bool is_stopping() const noexcept;

    /**
     * @brief Return true if the runtime is stopped.
     */
    [[nodiscard]] bool is_stopped() const noexcept;

    /**
     * @brief Return true if the runtime failed.
     */
    [[nodiscard]] bool is_failed() const noexcept;

    /**
     * @brief Return true if the runtime can be started.
     */
    [[nodiscard]] bool can_start() const noexcept;

    /**
     * @brief Return true if the runtime can be stopped.
     */
    [[nodiscard]] bool can_stop() const noexcept;

    /**
     * @brief Transition Created/Stopped to Starting.
     */
    [[nodiscard]] Error mark_starting() noexcept;

    /**
     * @brief Transition Starting to Running.
     */
    [[nodiscard]] Error mark_running() noexcept;

    /**
     * @brief Transition Running/Starting to Stopping.
     */
    [[nodiscard]] Error mark_stopping() noexcept;

    /**
     * @brief Transition any non-Failed state to Stopped.
     */
    [[nodiscard]] Error mark_stopped() noexcept;

    /**
     * @brief Transition to Failed.
     */
    void mark_failed() noexcept;

    /**
     * @brief Reset state back to Created.
     */
    void reset() noexcept;

  private:
    std::atomic<RuntimeLifecycleState> state_;
  };

  /**
   * @brief Convert RuntimeLifecycleState to a stable string.
   */
  [[nodiscard]] const char *to_string(
      RuntimeLifecycleState state) noexcept;

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_RUNTIME_STATE_HPP
