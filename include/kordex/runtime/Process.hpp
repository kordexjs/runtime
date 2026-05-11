/**
 *
 *  @file Process.hpp
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

#ifndef KORDEX_RUNTIME_PROCESS_HPP
#define KORDEX_RUNTIME_PROCESS_HPP

#include <string>
#include <vector>

#include <vix/process/Process.hpp>

#include <kordex/runtime/Result.hpp>
#include <kordex/runtime/RuntimeConfig.hpp>
#include <kordex/runtime/RuntimeResult.hpp>

namespace kordex::runtime
{
  /**
   * @enum ProcessStreamMode
   * @brief Runtime process stream handling mode.
   */
  enum class ProcessStreamMode
  {
    Inherit,
    Pipe,
    Null
  };

  /**
   * @struct ProcessOptions
   * @brief Options used when spawning or running a process from Kordex.
   */
  struct ProcessOptions
  {
    /**
     * @brief Standard input mode.
     */
    ProcessStreamMode stdin_mode{ProcessStreamMode::Inherit};

    /**
     * @brief Standard output mode.
     */
    ProcessStreamMode stdout_mode{ProcessStreamMode::Inherit};

    /**
     * @brief Standard error mode.
     */
    ProcessStreamMode stderr_mode{ProcessStreamMode::Inherit};

    /**
     * @brief Working directory for the process.
     *
     * Empty means current process directory.
     */
    std::string working_directory{};

    /**
     * @brief Whether the executable should be searched in PATH.
     */
    bool search_in_path{true};

    /**
     * @brief Whether the process should inherit the parent environment.
     */
    bool inherit_environment{true};

    /**
     * @brief Whether the process should be detached.
     */
    bool detach{false};
  };

  /**
   * @struct ProcessCommand
   * @brief Kordex process command description.
   */
  struct ProcessCommand
  {
    /**
     * @brief Executable program name or path.
     */
    std::string program{};

    /**
     * @brief Program arguments.
     */
    std::vector<std::string> args{};

    /**
     * @brief Process options.
     */
    ProcessOptions options{};

    /**
     * @brief Return true if the command has a program.
     */
    [[nodiscard]] bool valid() const noexcept;

    /**
     * @brief Convert to Vix process command.
     */
    [[nodiscard]] vix::process::Command to_vix_command() const;
  };

  /**
   * @struct ProcessOutput
   * @brief Captured process output.
   */
  struct ProcessOutput
  {
    /**
     * @brief Process exit code.
     */
    int exit_code{0};

    /**
     * @brief Captured stdout text.
     */
    std::string stdout_text{};

    /**
     * @brief Captured stderr text.
     */
    std::string stderr_text{};

    /**
     * @brief Return true when exit_code is zero.
     */
    [[nodiscard]] bool success() const noexcept;
  };

  /**
   * @class Process
   * @brief Kordex process facade.
   *
   * This class wraps the Vix process module and applies Kordex runtime
   * permission checks before executing external commands.
   */
  class Process
  {
  public:
    /**
     * @brief Construct with default runtime config.
     */
    Process();

    /**
     * @brief Construct with explicit runtime config.
     */
    explicit Process(RuntimeConfig config);

    /**
     * @brief Run a command to completion and capture output.
     */
    [[nodiscard]] Result<ProcessOutput> output(
        const ProcessCommand &command) const;

    /**
     * @brief Spawn a process and return the child handle.
     */
    [[nodiscard]] Result<vix::process::Child> spawn(
        const ProcessCommand &command) const;

    /**
     * @brief Wait for a child process.
     */
    [[nodiscard]] Result<int> wait(
        const vix::process::Child &child) const;

    /**
     * @brief Terminate a child process gracefully.
     */
    [[nodiscard]] Error terminate(
        const vix::process::Child &child) const;

    /**
     * @brief Kill a child process forcefully.
     */
    [[nodiscard]] Error kill(
        const vix::process::Child &child) const;

    /**
     * @brief Return whether process access is allowed.
     */
    [[nodiscard]] bool allowed() const noexcept;

    /**
     * @brief Return the runtime config used by this process facade.
     */
    [[nodiscard]] const RuntimeConfig &config() const noexcept;

  private:
    [[nodiscard]] Error check_allowed() const;

    RuntimeConfig config_;
  };

  /**
   * @brief Convert ProcessStreamMode to Vix PipeMode.
   */
  [[nodiscard]] vix::process::PipeMode to_vix_pipe_mode(
      ProcessStreamMode mode) noexcept;

  /**
   * @brief Convert ProcessStreamMode to a stable string.
   */
  [[nodiscard]] const char *to_string(
      ProcessStreamMode mode) noexcept;

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_PROCESS_HPP
