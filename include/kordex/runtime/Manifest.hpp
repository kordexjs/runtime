/**
 *
 *  @file Manifest.hpp
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

#ifndef KORDEX_RUNTIME_MANIFEST_HPP
#define KORDEX_RUNTIME_MANIFEST_HPP

#include <string>
#include <vector>

#include <vix/json/json.hpp>

#include <kordex/runtime/Clock.hpp>
#include <kordex/runtime/Result.hpp>

namespace kordex::runtime
{
  /**
   * @enum ManifestType
   * @brief Supported Kordex manifest file types.
   */
  enum class ManifestType
  {
    Unknown,
    PackageJson,
    KordexJson
  };

  /**
   * @struct Manifest
   * @brief Parsed runtime manifest.
   *
   * Manifest is used to represent metadata loaded from package.json,
   * kordex.json, or future Kordex runtime metadata files.
   */
  struct Manifest
  {
    /**
     * @brief Path used to load the manifest.
     */
    std::string path{};

    /**
     * @brief Manifest type.
     */
    ManifestType type{ManifestType::Unknown};

    /**
     * @brief Package or application name.
     */
    std::string name{};

    /**
     * @brief Package or application version.
     */
    std::string version{};

    /**
     * @brief Runtime entry file.
     */
    std::string entry{};

    /**
     * @brief Runtime environment name.
     */
    std::string environment{};

    /**
     * @brief Scripts declared by the manifest.
     */
    std::vector<std::string> scripts{};

    /**
     * @brief Raw parsed JSON document.
     */
    vix::json::Json raw{};

    /**
     * @brief Time when the manifest was loaded.
     */
    Timestamp loaded_at{};

    /**
     * @brief Load and parse a manifest file from disk.
     */
    [[nodiscard]] static Result<Manifest> load(
        const std::string &path);

    /**
     * @brief Parse a manifest from an already loaded JSON document.
     */
    [[nodiscard]] static Result<Manifest> from_json(
        vix::json::Json json,
        std::string path = {});

    /**
     * @brief Return true if this manifest has a usable entry file.
     */
    [[nodiscard]] bool has_entry() const noexcept;

    /**
     * @brief Return true if this manifest has a name.
     */
    [[nodiscard]] bool has_name() const noexcept;

    /**
     * @brief Return true if this manifest has a version.
     */
    [[nodiscard]] bool has_version() const noexcept;

    /**
     * @brief Return true if this manifest is package.json.
     */
    [[nodiscard]] bool is_package_json() const noexcept;

    /**
     * @brief Return true if this manifest is kordex.json.
     */
    [[nodiscard]] bool is_kordex_json() const noexcept;

    /**
     * @brief Validate manifest consistency.
     */
    [[nodiscard]] Error validate() const;
  };

  /**
   * @brief Detect manifest type from path.
   */
  [[nodiscard]] ManifestType detect_manifest_type(
      const std::string &path) noexcept;

  /**
   * @brief Convert ManifestType to a stable string.
   */
  [[nodiscard]] const char *to_string(
      ManifestType type) noexcept;

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_MANIFEST_HPP
