/**
 *
 *  @file SourceFile.cpp
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

#include <string>
#include <utility>

#include <vix/fs/IsFile.hpp>
#include <vix/fs/ReadText.hpp>
#include <vix/path/Extension.hpp>
#include <vix/path/Normalize.hpp>

#include <kordex/runtime/SourceFile.hpp>

namespace kordex::runtime
{
  namespace
  {
    [[nodiscard]] std::string normalize_path_or_original(
        const std::string &path)
    {
      auto normalized = vix::path::normalize(path);
      if (!normalized)
      {
        return path;
      }

      return normalized.value();
    }
  } // namespace

  SourceType detect_source_type(
      std::string_view path) noexcept
  {
    auto extension = vix::path::extension(path);
    if (!extension)
    {
      return SourceType::Unknown;
    }

    const std::string &ext = extension.value();

    if (ext == ".js" || ext == ".mjs" || ext == ".cjs")
    {
      return SourceType::JavaScript;
    }

    if (ext == ".ts" || ext == ".mts" || ext == ".cts")
    {
      return SourceType::TypeScript;
    }

    if (ext == ".json")
    {
      return SourceType::Json;
    }

    return SourceType::Unknown;
  }

  const char *to_string(
      SourceType type) noexcept
  {
    switch (type)
    {
    case SourceType::Unknown:
      return "unknown";
    case SourceType::JavaScript:
      return "javascript";
    case SourceType::TypeScript:
      return "typescript";
    case SourceType::Json:
      return "json";
    }

    return "unknown";
  }

  Result<SourceFile> SourceFile::load(
      const std::string &path)
  {
    if (path.empty())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "source path cannot be empty");
    }

    const std::string normalized_path = normalize_path_or_original(path);

    auto is_file = vix::fs::is_file(normalized_path);
    if (!is_file)
    {
      return make_runtime_error(
          RuntimeErrorCode::SourceNotFound,
          std::string(is_file.error().message()));
    }

    if (!is_file.value())
    {
      return make_runtime_error(
          RuntimeErrorCode::SourceNotFound,
          "source path is not a file: " + normalized_path);
    }

    auto content = vix::fs::read_text(normalized_path);
    if (!content)
    {
      return make_runtime_error(
          RuntimeErrorCode::SourceReadFailed,
          std::string(content.error().message()));
    }

    SourceFile source;
    source.original_path = path;
    source.path = normalized_path;
    source.type = detect_source_type(normalized_path);
    source.content = std::move(content.value());
    source.loaded_at = Clock::now();

    const auto validation = source.validate();
    if (validation)
    {
      return validation;
    }

    return source;
  }

  Result<SourceFile> SourceFile::from_content(
      std::string path,
      std::string content)
  {
    if (path.empty())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "source path cannot be empty");
    }

    SourceFile source;
    source.original_path = path;
    source.path = normalize_path_or_original(path);
    source.type = detect_source_type(source.path);
    source.content = std::move(content);
    source.loaded_at = Clock::now();

    const auto validation = source.validate();
    if (validation)
    {
      return validation;
    }

    return source;
  }

  bool SourceFile::empty() const noexcept
  {
    return content.empty();
  }

  std::size_t SourceFile::size() const noexcept
  {
    return content.size();
  }

  bool SourceFile::is_javascript() const noexcept
  {
    return type == SourceType::JavaScript;
  }

  bool SourceFile::is_typescript() const noexcept
  {
    return type == SourceType::TypeScript;
  }

  bool SourceFile::is_json() const noexcept
  {
    return type == SourceType::Json;
  }

  bool SourceFile::executable() const noexcept
  {
    return is_javascript() || is_typescript();
  }

  Error SourceFile::validate() const
  {
    if (path.empty())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "source path cannot be empty");
    }

    if (type == SourceType::Unknown)
    {
      return make_runtime_error(
          RuntimeErrorCode::UnsupportedSourceType,
          "unsupported source file type: " + path);
    }

    return ok();
  }

} // namespace kordex::runtime
