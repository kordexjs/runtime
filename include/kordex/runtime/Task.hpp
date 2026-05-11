/**
 *
 *  @file Task.hpp
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

#ifndef KORDEX_RUNTIME_TASK_HPP
#define KORDEX_RUNTIME_TASK_HPP

#include <functional>
#include <string>

#include <vix/runtime/Task.hpp>

#include <kordex/runtime/Clock.hpp>
#include <kordex/runtime/Result.hpp>
#include <kordex/runtime/RuntimeResult.hpp>

namespace kordex::runtime
{
  /**
   * @enum TaskStatus
   * @brief High-level Kordex task execution status.
   */
  enum class TaskStatus
  {
    Pending,
    Running,
    Completed,
    Failed,
    Cancelled
  };

  /**
   * @brief User-facing task function.
   *
   * Kordex keeps the public runtime task API simple.
   * Internally, this is adapted to vix::runtime::TaskFn.
   */
  using TaskFunction = std::function<void()>;

  /**
   * @struct TaskOptions
   * @brief Options attached to a runtime task.
   */
  struct TaskOptions
  {
    /**
     * @brief Human-readable task name.
     */
    std::string name{};

    /**
     * @brief Whether task failure should be reported as critical.
     */
    bool critical{false};

    /**
     * @brief Whether the task may block.
     */
    bool may_block{false};
  };

  /**
   * @struct TaskInfo
   * @brief Runtime task metadata and result state.
   */
  struct TaskInfo
  {
    /**
     * @brief Task name.
     */
    std::string name{};

    /**
     * @brief Current task status.
     */
    TaskStatus status{TaskStatus::Pending};

    /**
     * @brief Task start timestamp.
     */
    Timestamp started_at{};

    /**
     * @brief Task finish timestamp.
     */
    Timestamp finished_at{};

    /**
     * @brief Task duration.
     */
    Duration duration{};

    /**
     * @brief Optional task error.
     */
    Error error{};

    /**
     * @brief Return true if the task completed successfully.
     */
    [[nodiscard]] bool succeeded() const noexcept;

    /**
     * @brief Return true if the task failed.
     */
    [[nodiscard]] bool failed() const noexcept;

    /**
     * @brief Return true if the task was cancelled.
     */
    [[nodiscard]] bool cancelled() const noexcept;

    /**
     * @brief Return true if the task has finished.
     */
    [[nodiscard]] bool finished() const noexcept;

    /**
     * @brief Mark the task as running.
     */
    void mark_running() noexcept;

    /**
     * @brief Mark the task as completed.
     */
    void mark_completed() noexcept;

    /**
     * @brief Mark the task as failed.
     */
    void mark_failed(Error error_value);

    /**
     * @brief Mark the task as cancelled.
     */
    void mark_cancelled(Error error_value);

  private:
    void mark_finished_time() noexcept;
  };

  /**
   * @class Task
   * @brief Kordex runtime task wrapper.
   *
   * Task wraps a simple void function and exposes helpers to execute it,
   * track status, and adapt it to Vix runtime tasks.
   */
  class Task
  {
  public:
    /**
     * @brief Construct an empty invalid task.
     */
    Task() = default;

    /**
     * @brief Construct a task from function and options.
     */
    Task(TaskFunction function, TaskOptions options = {});

    /**
     * @brief Return true if the task has a callable function.
     */
    [[nodiscard]] bool valid() const noexcept;

    /**
     * @brief Return task options.
     */
    [[nodiscard]] const TaskOptions &options() const noexcept;

    /**
     * @brief Return task info.
     */
    [[nodiscard]] const TaskInfo &info() const noexcept;

    /**
     * @brief Execute the task immediately.
     */
    [[nodiscard]] RuntimeResult run();

    /**
     * @brief Convert this task to a Vix runtime task function.
     */
    [[nodiscard]] vix::runtime::TaskFn to_vix_task();

  private:
    TaskFunction function_{};
    TaskOptions options_{};
    TaskInfo info_{};
  };

  /**
   * @brief Convert TaskStatus to a stable string.
   */
  [[nodiscard]] const char *to_string(
      TaskStatus status) noexcept;

  /**
   * @brief Convert Kordex TaskStatus to RuntimeExitStatus.
   */
  [[nodiscard]] RuntimeExitStatus to_runtime_exit_status(
      TaskStatus status) noexcept;

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_TASK_HPP
