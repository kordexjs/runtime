/**
 *
 *  @file Manifest.cpp
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

#include <exception>
#include <string>
#include <utility>

#include <vix/fs/ReadText.hpp>
#include <vix/json/json.hpp>
#include <vix/json/loads.hpp>
#include <vix/path/Filename.hpp>

#include <kordex/runtime/Manifest.hpp>

namespace kordex::runtime
{
  namespace
  {
    [[nodiscard]] std::string get_string_or_empty(
        const vix::json::Json &json,
        const char *key)
    {
      if (!json.is_object() || !json.contains(key))
      {
        return {};
      }

      const auto &value = json.at(key);
      if (!value.is_string())
      {
        return {};
      }

      return value.get<std::string>();
    }

    [[nodiscard]] std::string read_entry(
        const vix::json::Json &json)
    {
      if (!json.is_object())
      {
        return {};
      }

      if (json.contains("entry") && json.at("entry").is_string())
      {
        return json.at("entry").get<std::string>();
      }

      if (json.contains("main") && json.at("main").is_string())
      {
        return json.at("main").get<std::string>();
      }

      if (json.contains("module") && json.at("module").is_string())
      {
        return json.at("module").get<std::string>();
      }

      return {};
    }

    [[nodiscard]] std::vector<std::string> read_scripts(
        const vix::json::Json &json)
    {
      std::vector<std::string> scripts;

      if (!json.is_object() ||
          !json.contains("scripts") ||
          !json.at("scripts").is_object())
      {
        return scripts;
      }

      for (const auto &[key, value] : json.at("scripts").items())
      {
        (void)value;
        scripts.push_back(key);
      }

      return scripts;
    }
  } // namespace

  ManifestType detect_manifest_type(
      const std::string &path) noexcept
  {
    auto filename = vix::path::filename(path);
    if (!filename)
    {
      return ManifestType::Unknown;
    }

    if (filename.value() == "package.json")
    {
      return ManifestType::PackageJson;
    }

    if (filename.value() == "kordex.json")
    {
      return ManifestType::KordexJson;
    }

    return ManifestType::Unknown;
  }

  const char *to_string(
      ManifestType type) noexcept
  {
    switch (type)
    {
    case ManifestType::Unknown:
      return "unknown";
    case ManifestType::PackageJson:
      return "package_json";
    case ManifestType::KordexJson:
      return "kordex_json";
    }

    return "unknown";
  }

  Result<Manifest> Manifest::load(
      const std::string &path)
  {
    if (path.empty())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "manifest path cannot be empty");
    }

    auto content = vix::fs::read_text(path);
    if (!content)
    {
      return make_runtime_error(
          RuntimeErrorCode::ManifestLoadFailed,
          std::string(content.error().message()));
    }

    auto json = vix::json::try_loads(content.value());
    if (!json)
    {
      return make_runtime_error(
          RuntimeErrorCode::ManifestParseFailed,
          "failed to parse manifest JSON: " + path);
    }

    return from_json(std::move(*json), path);
  }

  Result<Manifest> Manifest::from_json(
      vix::json::Json json,
      std::string path)
  {
    if (!json.is_object())
    {
      return make_runtime_error(
          RuntimeErrorCode::ManifestParseFailed,
          "manifest root must be a JSON object");
    }

    Manifest manifest;
    manifest.path = std::move(path);
    manifest.type = detect_manifest_type(manifest.path);
    manifest.name = get_string_or_empty(json, "name");
    manifest.version = get_string_or_empty(json, "version");
    manifest.entry = read_entry(json);
    manifest.environment = get_string_or_empty(json, "environment");
    manifest.scripts = read_scripts(json);
    manifest.raw = std::move(json);
    manifest.loaded_at = Clock::now();

    const auto validation = manifest.validate();
    if (validation)
    {
      return validation;
    }

    return manifest;
  }

  bool Manifest::has_entry() const noexcept
  {
    return !entry.empty();
  }

  bool Manifest::has_name() const noexcept
  {
    return !name.empty();
  }

  bool Manifest::has_version() const noexcept
  {
    return !version.empty();
  }

  bool Manifest::is_package_json() const noexcept
  {
    return type == ManifestType::PackageJson;
  }

  bool Manifest::is_kordex_json() const noexcept
  {
    return type == ManifestType::KordexJson;
  }

  Error Manifest::validate() const
  {
    if (raw.is_null())
    {
      return make_runtime_error(
          RuntimeErrorCode::ManifestParseFailed,
          "manifest raw JSON cannot be null");
    }

    if (!raw.is_object())
    {
      return make_runtime_error(
          RuntimeErrorCode::ManifestParseFailed,
          "manifest raw JSON must be an object");
    }

    return ok();
  }

} // namespace kordex::runtime
