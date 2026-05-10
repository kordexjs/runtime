/**
 *
 *  @file SourceFile.hpp
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

#ifndef KORDEX_RUNTIME_SOURCE_FILE_HPP
#define KORDEX_RUNTIME_SOURCE_FILE_HPP

#include <cstddef>
#include <string>
#include <string_view>

#include <kordex/runtime/Clock.hpp>
#include <kordex/runtime/Result.hpp>

namespace kordex::runtime
{
  /**
   * @enum SourceType
   * @brief Supported source file types.
   */
  enum class SourceType
  {
    Unknown,
    JavaScript,
    TypeScript,
    Json
  };

  /**
   * @struct SourceFile
   * @brief Loaded runtime source file.
   *
   * SourceFile represents a source file after path normalization and disk
   * loading. It is used by Runtime, ModuleResolver, and future bindings.
   */
  struct SourceFile
  {
    /**
     * @brief Original path provided by the caller.
     */
    std::string original_path{};

    /**
     * @brief Normalized path used internally by the runtime.
     */
    std::string path{};

    /**
     * @brief Source file type.
     */
    SourceType type{SourceType::Unknown};

    /**
     * @brief Source file content.
     */
    std::string content{};

    /**
     * @brief Time when the file was loaded.
     */
    Timestamp loaded_at{};

    /**
     * @brief Load a source file from disk.
     */
    [[nodiscard]] static Result<SourceFile> load(
        const std::string &path);

    /**
     * @brief Create a source file from already loaded content.
     */
    [[nodiscard]] static Result<SourceFile> from_content(
        std::string path,
        std::string content);

    /**
     * @brief Return true if source content is empty.
     */
    [[nodiscard]] bool empty() const noexcept;

    /**
     * @brief Return source content size in bytes.
     */
    [[nodiscard]] std::size_t size() const noexcept;

    /**
     * @brief Return true if the source type is JavaScript.
     */
    [[nodiscard]] bool is_javascript() const noexcept;

    /**
     * @brief Return true if the source type is TypeScript.
     */
    [[nodiscard]] bool is_typescript() const noexcept;

    /**
     * @brief Return true if the source type is JSON.
     */
    [[nodiscard]] bool is_json() const noexcept;

    /**
     * @brief Return true if this source file can be executed directly.
     */
    [[nodiscard]] bool executable() const noexcept;

    /**
     * @brief Validate the source file.
     */
    [[nodiscard]] Error validate() const;
  };

  /**
   * @brief Detect source type from path extension.
   */
  [[nodiscard]] SourceType detect_source_type(
      std::string_view path) noexcept;

  /**
   * @brief Convert SourceType to a stable string.
   */
  [[nodiscard]] const char *to_string(
      SourceType type) noexcept;

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_SOURCE_FILE_HPP
