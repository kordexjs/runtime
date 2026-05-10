/**
 *
 *  @file Cancellation.hpp
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

#ifndef KORDEX_RUNTIME_CANCELLATION_HPP
#define KORDEX_RUNTIME_CANCELLATION_HPP

#include <vix/async/core/cancel.hpp>
#include <kordex/runtime/Error.hpp>

namespace kordex::runtime
{
  /**
   * @brief Runtime cancellation token.
   *
   * This is the read-only cancellation observer used by runtime tasks,
   * timers, process operations, and shutdown logic.
   */
  using CancelToken = vix::async::core::cancel_token;

  /**
   * @brief Runtime cancellation source.
   *
   * This owns the cancellation state and can request cancellation.
   */
  using CancelSource = vix::async::core::cancel_source;

  /**
   * @class Cancellation
   * @brief Small runtime-owned cancellation controller.
   *
   * Cancellation wraps the Vix async cancellation primitives and provides
   * a Kordex-facing API for runtime shutdown and task interruption.
   */
  class Cancellation
  {
  public:
    /**
     * @brief Construct a new cancellation controller.
     */
    Cancellation();

    /**
     * @brief Return a token observing this cancellation source.
     */
    [[nodiscard]] CancelToken token() const noexcept;

    /**
     * @brief Request cancellation.
     */
    void request_cancel() noexcept;

    /**
     * @brief Return true if cancellation has been requested.
     */
    [[nodiscard]] bool is_cancelled() const noexcept;

    /**
     * @brief Return true if this controller can issue cancellation.
     */
    [[nodiscard]] bool can_cancel() const noexcept;

    /**
     * @brief Return a structured cancellation error.
     */
    [[nodiscard]] Error error(
        const char *message = "runtime operation was cancelled") const;

  private:
    CancelSource source_;
  };

  /**
   * @brief Return a standard runtime cancellation error.
   */
  [[nodiscard]] Error make_cancelled_error(
      const char *message = "runtime operation was cancelled");

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_CANCELLATION_HPP
