/**
 *
 *  @file ModuleResolver.hpp
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

#ifndef KORDEX_RUNTIME_MODULE_RESOLVER_HPP
#define KORDEX_RUNTIME_MODULE_RESOLVER_HPP

#include <string>
#include <vector>

#include <kordex/runtime/Clock.hpp>
#include <kordex/runtime/ModuleId.hpp>
#include <kordex/runtime/Result.hpp>
#include <kordex/runtime/SourceFile.hpp>

namespace kordex::runtime
{
  /**
   * @enum ResolvedModuleKind
   * @brief Type of resolved runtime module.
   */
  enum class ResolvedModuleKind
  {
    Unknown,
    File,
    Package,
    Builtin
  };

  /**
   * @struct ResolvedModule
   * @brief Result of resolving a module specifier.
   */
  struct ResolvedModule
  {
    /**
     * @brief Original parsed module id.
     */
    ModuleId id{};

    /**
     * @brief Resolved module kind.
     */
    ResolvedModuleKind kind{ResolvedModuleKind::Unknown};

    /**
     * @brief Resolved absolute or normalized path for file modules.
     */
    std::string path{};

    /**
     * @brief Package name for package modules.
     */
    std::string package_name{};

    /**
     * @brief Builtin module name for builtin modules.
     */
    std::string builtin_name{};

    /**
     * @brief Resolved source type when the target is a file.
     */
    SourceType source_type{SourceType::Unknown};

    /**
     * @brief Time when the module was resolved.
     */
    Timestamp resolved_at{};

    /**
     * @brief Return true if this resolved module is valid.
     */
    [[nodiscard]] bool valid() const noexcept;

    /**
     * @brief Return true if this resolved module is a file.
     */
    [[nodiscard]] bool is_file() const noexcept;

    /**
     * @brief Return true if this resolved module is a package.
     */
    [[nodiscard]] bool is_package() const noexcept;

    /**
     * @brief Return true if this resolved module is builtin.
     */
    [[nodiscard]] bool is_builtin() const noexcept;

    /**
     * @brief Validate this resolved module.
     */
    [[nodiscard]] Error validate() const;
  };

  /**
   * @struct ModuleResolverOptions
   * @brief Options controlling module resolution.
   */
  struct ModuleResolverOptions
  {
    /**
     * @brief Base directory used to resolve relative imports.
     */
    std::string base_dir{"."};

    /**
     * @brief Extra search paths for package resolution.
     */
    std::vector<std::string> package_paths{};

    /**
     * @brief Supported runtime extensions, in lookup order.
     */
    std::vector<std::string> extensions{
        ".js",
        ".mjs",
        ".cjs",
        ".ts",
        ".mts",
        ".cts",
        ".json"};

    /**
     * @brief Whether builtin modules are allowed.
     */
    bool allow_builtins{true};

    /**
     * @brief Whether package imports are allowed.
     */
    bool allow_packages{true};

    /**
     * @brief Whether missing file extensions should be resolved automatically.
     */
    bool resolve_extensions{true};

    /**
     * @brief Whether index files should be resolved for directories.
     */
    bool resolve_index{true};
  };

  /**
   * @class ModuleResolver
   * @brief Runtime module resolver for Kordex.
   *
   * ModuleResolver resolves module specifiers into normalized runtime targets.
   * It handles:
   * - relative source files
   * - absolute source files
   * - builtin modules like kordex:fs
   * - package names for future package loading
   */
  class ModuleResolver
  {
  public:
    /**
     * @brief Construct with default options.
     */
    ModuleResolver();

    /**
     * @brief Construct with explicit options.
     */
    explicit ModuleResolver(ModuleResolverOptions options);

    /**
     * @brief Resolve a raw module specifier.
     */
    [[nodiscard]] Result<ResolvedModule> resolve(
        const std::string &specifier) const;

    /**
     * @brief Resolve an already parsed module id.
     */
    [[nodiscard]] Result<ResolvedModule> resolve(
        const ModuleId &id) const;

    /**
     * @brief Return resolver options.
     */
    [[nodiscard]] const ModuleResolverOptions &options() const noexcept;

    /**
     * @brief Update resolver options.
     */
    void set_options(ModuleResolverOptions options);

  private:
    [[nodiscard]] Result<ResolvedModule> resolve_file_module(
        const ModuleId &id) const;

    [[nodiscard]] Result<ResolvedModule> resolve_package_module(
        const ModuleId &id) const;

    [[nodiscard]] Result<ResolvedModule> resolve_builtin_module(
        const ModuleId &id) const;

    ModuleResolverOptions options_;
  };

  /**
   * @brief Convert ResolvedModuleKind to a stable string.
   */
  [[nodiscard]] const char *to_string(
      ResolvedModuleKind kind) noexcept;

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_MODULE_RESOLVER_HPP
