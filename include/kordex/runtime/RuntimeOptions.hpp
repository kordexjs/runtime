/**
 *
 *  @file RuntimeOptions.hpp
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

#ifndef KORDEX_RUNTIME_RUNTIME_OPTIONS_HPP
#define KORDEX_RUNTIME_RUNTIME_OPTIONS_HPP

#include <cstddef>
#include <string>
#include <vector>

#include <kordex/runtime/Clock.hpp>

namespace kordex::runtime
{
  /**
   * @enum RuntimeMode
   * @brief High-level runtime execution mode.
   */
  enum class RuntimeMode
  {
    Development,
    Production,
    Test
  };

  /**
   * @enum PermissionMode
   * @brief Global permission policy for runtime capabilities.
   */
  enum class PermissionMode
  {
    Strict,
    Relaxed
  };

  /**
   * @struct RuntimeOptions
   * @brief Explicit options used to configure a Kordex runtime instance.
   *
   * RuntimeOptions contains direct user or caller-provided settings.
   * Environment-derived settings are handled later by RuntimeConfig.
   */
  struct RuntimeOptions
  {
    /**
     * @brief Runtime mode.
     */
    RuntimeMode mode{RuntimeMode::Development};

    /**
     * @brief Global permission mode.
     */
    PermissionMode permission_mode{PermissionMode::Strict};

    /**
     * @brief Number of runtime worker threads.
     *
     * Zero means "use runtime default".
     */
    std::size_t workers{0};

    /**
     * @brief Optional runtime cache directory.
     */
    std::string cache_dir{};

    /**
     * @brief Optional current working directory override.
     *
     * Empty means "use process current working directory".
     */
    std::string working_directory{};

    /**
     * @brief Arguments passed to the executed user program.
     */
    std::vector<std::string> args{};

    /**
     * @brief Default execution timeout.
     *
     * Zero means no timeout.
     */
    Duration timeout{};

    /**
     * @brief Whether filesystem access is allowed.
     */
    bool allow_fs{true};

    /**
     * @brief Whether network access is allowed.
     */
    bool allow_net{false};

    /**
     * @brief Whether process spawning is allowed.
     */
    bool allow_process{false};

    /**
     * @brief Whether environment access is allowed.
     */
    bool allow_env{true};

    /**
     * @brief Whether diagnostics are enabled.
     */
    bool diagnostics{true};

    /**
     * @brief Whether debug mode is enabled.
     */
    bool debug{false};

    /**
     * @brief Return default runtime options.
     */
    [[nodiscard]] static RuntimeOptions defaults();

    /**
     * @brief Return development runtime options.
     */
    [[nodiscard]] static RuntimeOptions development();

    /**
     * @brief Return production runtime options.
     */
    [[nodiscard]] static RuntimeOptions production();

    /**
     * @brief Return test runtime options.
     */
    [[nodiscard]] static RuntimeOptions test();

    /**
     * @brief Return true if timeout is enabled.
     */
    [[nodiscard]] bool has_timeout() const noexcept;

    /**
     * @brief Return true if a custom cache directory is configured.
     */
    [[nodiscard]] bool has_cache_dir() const noexcept;

    /**
     * @brief Return true if a custom working directory is configured.
     */
    [[nodiscard]] bool has_working_directory() const noexcept;
  };

  /**
   * @brief Convert RuntimeMode to a stable string.
   */
  [[nodiscard]] const char *to_string(RuntimeMode mode) noexcept;

  /**
   * @brief Convert PermissionMode to a stable string.
   */
  [[nodiscard]] const char *to_string(PermissionMode mode) noexcept;

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_RUNTIME_OPTIONS_HPP
