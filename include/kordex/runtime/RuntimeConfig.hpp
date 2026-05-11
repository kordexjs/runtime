/**
 *
 *  @file RuntimeConfig.hpp
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

#ifndef KORDEX_RUNTIME_RUNTIME_CONFIG_HPP
#define KORDEX_RUNTIME_RUNTIME_CONFIG_HPP

#include <cstddef>
#include <string>

#include <kordex/runtime/Error.hpp>
#include <kordex/runtime/Result.hpp>
#include <kordex/runtime/RuntimeOptions.hpp>

namespace kordex::runtime
{
  /**
   * @struct RuntimeConfig
   * @brief Final runtime configuration after merging options and environment.
   *
   * RuntimeOptions represents caller-provided values.
   * RuntimeConfig represents the normalized configuration used internally
   * by the runtime.
   */
  struct RuntimeConfig
  {
    RuntimeMode mode{RuntimeMode::Development};
    PermissionMode permission_mode{PermissionMode::Strict};

    std::size_t workers{1};

    std::string cache_dir{};
    std::string working_directory{};
    std::string environment{"development"};

    Duration timeout{};

    bool allow_fs{true};
    bool allow_net{false};
    bool allow_process{false};
    bool allow_env{true};

    bool diagnostics{true};
    bool debug{false};

    /**
     * @brief Build a RuntimeConfig from explicit RuntimeOptions.
     */
    [[nodiscard]] static Result<RuntimeConfig> from_options(
        const RuntimeOptions &options);

    /**
     * @brief Build a RuntimeConfig from RuntimeOptions and KORDEX_* env vars.
     */
    [[nodiscard]] static Result<RuntimeConfig> from_environment(
        const RuntimeOptions &base = RuntimeOptions::defaults());

    /**
     * @brief Return true if timeout is enabled.
     */
    [[nodiscard]] bool has_timeout() const noexcept;

    /**
     * @brief Return true if a cache directory is configured.
     */
    [[nodiscard]] bool has_cache_dir() const noexcept;

    /**
     * @brief Return true if a working directory is configured.
     */
    [[nodiscard]] bool has_working_directory() const noexcept;

    /**
     * @brief Validate the final runtime configuration.
     */
    [[nodiscard]] Error validate() const;
  };

  /**
   * @brief Parse a runtime mode string.
   */
  [[nodiscard]] Result<RuntimeMode> parse_runtime_mode(
      const std::string &value);

  /**
   * @brief Parse a permission mode string.
   */
  [[nodiscard]] Result<PermissionMode> parse_permission_mode(
      const std::string &value);

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_RUNTIME_CONFIG_HPP
