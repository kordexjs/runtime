/**
 *
 *  @file ModuleId.hpp
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

#ifndef KORDEX_RUNTIME_MODULE_ID_HPP
#define KORDEX_RUNTIME_MODULE_ID_HPP

#include <string>
#include <string_view>

#include <kordex/runtime/Result.hpp>

namespace kordex::runtime
{
  /**
   * @enum ModuleIdKind
   * @brief Type of module identifier.
   */
  enum class ModuleIdKind
  {
    Unknown,
    Relative,
    Absolute,
    Package,
    Builtin
  };

  /**
   * @class ModuleId
   * @brief Parsed module identifier used by the runtime resolver.
   *
   * ModuleId represents the raw specifier passed by user code.
   *
   * Examples:
   * - ./app.js        -> Relative
   * - ../lib/mod.js   -> Relative
   * - /app/main.js    -> Absolute
   * - kordex:fs       -> Builtin
   * - react           -> Package
   * - @scope/pkg      -> Package
   */
  class ModuleId
  {
  public:
    /**
     * @brief Construct an empty invalid module id.
     */
    ModuleId() = default;

    /**
     * @brief Construct a module id from kind and specifier.
     */
    ModuleId(ModuleIdKind kind, std::string specifier);

    /**
     * @brief Parse a module id from a raw specifier.
     */
    [[nodiscard]] static Result<ModuleId> parse(
        std::string specifier);

    /**
     * @brief Return the module id kind.
     */
    [[nodiscard]] ModuleIdKind kind() const noexcept;

    /**
     * @brief Return the original module specifier.
     */
    [[nodiscard]] const std::string &specifier() const noexcept;

    /**
     * @brief Return true if the module id is valid.
     */
    [[nodiscard]] bool valid() const noexcept;

    /**
     * @brief Return true if the module id is relative.
     */
    [[nodiscard]] bool is_relative() const noexcept;

    /**
     * @brief Return true if the module id is absolute.
     */
    [[nodiscard]] bool is_absolute() const noexcept;

    /**
     * @brief Return true if the module id is a package import.
     */
    [[nodiscard]] bool is_package() const noexcept;

    /**
     * @brief Return true if the module id targets a builtin Kordex module.
     */
    [[nodiscard]] bool is_builtin() const noexcept;

    /**
     * @brief Return true if the module id needs filesystem resolution.
     */
    [[nodiscard]] bool needs_filesystem_resolution() const noexcept;

    /**
     * @brief Return builtin module name without the kordex: prefix.
     */
    [[nodiscard]] std::string builtin_name() const;

    /**
     * @brief Validate this module id.
     */
    [[nodiscard]] Error validate() const;

  private:
    ModuleIdKind kind_{ModuleIdKind::Unknown};
    std::string specifier_{};
  };

  /**
   * @brief Detect module id kind from a raw specifier.
   */
  [[nodiscard]] ModuleIdKind detect_module_id_kind(
      std::string_view specifier) noexcept;

  /**
   * @brief Convert ModuleIdKind to a stable string.
   */
  [[nodiscard]] const char *to_string(
      ModuleIdKind kind) noexcept;

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_MODULE_ID_HPP
