/**
 *
 *  @file RuntimeResult.hpp
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

#ifndef KORDEX_RUNTIME_RUNTIME_RESULT_HPP
#define KORDEX_RUNTIME_RUNTIME_RESULT_HPP

#include <string>

#include <kordex/runtime/Clock.hpp>
#include <kordex/runtime/Error.hpp>
#include <kordex/runtime/Result.hpp>

namespace kordex::runtime
{
  /**
   * @enum RuntimeExitStatus
   * @brief High-level execution status for a runtime operation.
   */
  enum class RuntimeExitStatus
  {
    Success,
    Failed,
    Cancelled,
    Timeout
  };

  /**
   * @struct RuntimeResult
   * @brief Result produced after a runtime execution.
   *
   * RuntimeResult is used for high-level execution flows such as:
   * - running a source file
   * - executing a module
   * - running a process-backed runtime task
   *
   * It stores both status information and timing metadata.
   */
  struct RuntimeResult
  {
    /**
     * @brief Final runtime status.
     */
    RuntimeExitStatus status{RuntimeExitStatus::Success};

    /**
     * @brief Process-style exit code.
     *
     * By convention:
     * - 0 means success
     * - non-zero means failure
     */
    int exit_code{0};

    /**
     * @brief Optional structured error.
     */
    Error error{};

    /**
     * @brief Optional human-readable output.
     */
    std::string output{};

    /**
     * @brief Optional human-readable stderr output.
     */
    std::string error_output{};

    /**
     * @brief Wall-clock start timestamp.
     */
    Timestamp started_at{};

    /**
     * @brief Wall-clock finish timestamp.
     */
    Timestamp finished_at{};

    /**
     * @brief Measured runtime duration.
     */
    Duration duration{};

    /**
     * @brief Build a successful result.
     */
    [[nodiscard]] static RuntimeResult success(
        std::string output = {});

    /**
     * @brief Build a failed result from an error.
     */
    [[nodiscard]] static RuntimeResult failure(
        Error error,
        int exit_code = 1);

    /**
     * @brief Build a cancelled result.
     */
    [[nodiscard]] static RuntimeResult cancelled(
        std::string message = "runtime operation was cancelled");

    /**
     * @brief Build a timeout result.
     */
    [[nodiscard]] static RuntimeResult timeout(
        std::string message = "runtime operation timed out");

    /**
     * @brief Mark this result as started now.
     */
    void mark_started() noexcept;

    /**
     * @brief Mark this result as finished now.
     */
    void mark_finished() noexcept;

    /**
     * @brief Return true when the runtime operation succeeded.
     */
    [[nodiscard]] bool succeeded() const noexcept;

    /**
     * @brief Return true when the runtime operation failed.
     */
    [[nodiscard]] bool failed() const noexcept;

    /**
     * @brief Return true when the runtime operation was cancelled.
     */
    [[nodiscard]] bool was_cancelled() const noexcept;

    /**
     * @brief Return true when the runtime operation timed out.
     */
    [[nodiscard]] bool timed_out() const noexcept;

    /**
     * @brief Return true if timing metadata is available.
     */
    [[nodiscard]] bool has_timing() const noexcept;
  };

  /**
   * @brief Result type used by operations returning RuntimeResult.
   */
  using RuntimeExecutionResult = Result<RuntimeResult>;

  /**
   * @brief Convert RuntimeExitStatus to a stable string.
   */
  [[nodiscard]] const char *to_string(RuntimeExitStatus status) noexcept;

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_RUNTIME_RESULT_HPP
