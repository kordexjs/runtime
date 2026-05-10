/**
 *
 *  @file test_manifest.cpp
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

#include <filesystem>
#include <fstream>
#include <string_view>

#include <kordex/runtime/Manifest.hpp>

namespace
{
  bool expect_true(bool condition, const char *message)
  {
    if (!condition)
    {
      (void)message;
      return false;
    }

    return true;
  }

  std::filesystem::path test_dir()
  {
    return std::filesystem::temp_directory_path() /
           "kordex_runtime_manifest_tests";
  }

  std::filesystem::path write_test_file(
      const std::string &filename,
      const std::string &content)
  {
    const auto dir = test_dir();
    std::filesystem::create_directories(dir);

    const auto path = dir / filename;

    std::ofstream out(path, std::ios::trunc);
    out << content;

    return path;
  }

  void cleanup_test_files()
  {
    std::error_code ec;
    std::filesystem::remove_all(test_dir(), ec);
  }

  bool test_detect_manifest_type()
  {
    return expect_true(
               kordex::runtime::detect_manifest_type("package.json") ==
                   kordex::runtime::ManifestType::PackageJson,
               "package.json should be detected") &&
           expect_true(
               kordex::runtime::detect_manifest_type("/tmp/app/kordex.json") ==
                   kordex::runtime::ManifestType::KordexJson,
               "kordex.json should be detected") &&
           expect_true(
               kordex::runtime::detect_manifest_type("config.json") ==
                   kordex::runtime::ManifestType::Unknown,
               "unknown manifest should be detected");
  }

  bool test_manifest_type_to_string()
  {
    using kordex::runtime::ManifestType;

    return expect_true(
               std::string_view(kordex::runtime::to_string(ManifestType::Unknown)) == "unknown",
               "unknown should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(ManifestType::PackageJson)) == "package_json",
               "package_json should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(ManifestType::KordexJson)) == "kordex_json",
               "kordex_json should convert to string");
  }

  bool test_from_json_package_manifest()
  {
    vix::json::Json json = {
        {"name", "demo-app"},
        {"version", "0.1.0"},
        {"main", "src/main.js"},
        {"environment", "test"},
        {"scripts",
         {
             {"dev", "kordex run src/main.js"},
             {"test", "kordex test"},
         }},
    };

    auto result = kordex::runtime::Manifest::from_json(
        json,
        "package.json");

    if (!result)
    {
      return false;
    }

    const auto &manifest = result.value();

    return expect_true(
               manifest.is_package_json(),
               "manifest should be package.json") &&
           expect_true(
               manifest.name == "demo-app",
               "manifest name should be parsed") &&
           expect_true(
               manifest.version == "0.1.0",
               "manifest version should be parsed") &&
           expect_true(
               manifest.entry == "src/main.js",
               "manifest entry should use main") &&
           expect_true(
               manifest.environment == "test",
               "manifest environment should be parsed") &&
           expect_true(
               manifest.scripts.size() == 2,
               "manifest scripts should be parsed") &&
           expect_true(
               !manifest.loaded_at.is_zero(),
               "manifest loaded_at should be set") &&
           expect_true(
               manifest.has_name(),
               "manifest should report name") &&
           expect_true(
               manifest.has_version(),
               "manifest should report version") &&
           expect_true(
               manifest.has_entry(),
               "manifest should report entry");
  }

  bool test_from_json_kordex_manifest()
  {
    vix::json::Json json = {
        {"name", "kordex-demo"},
        {"version", "1.0.0"},
        {"entry", "app.js"},
        {"environment", "development"},
    };

    auto result = kordex::runtime::Manifest::from_json(
        json,
        "kordex.json");

    if (!result)
    {
      return false;
    }

    const auto &manifest = result.value();

    return expect_true(
               manifest.is_kordex_json(),
               "manifest should be kordex.json") &&
           expect_true(
               manifest.entry == "app.js",
               "manifest entry should use entry") &&
           expect_true(
               manifest.environment == "development",
               "manifest environment should be parsed");
  }

  bool test_from_json_rejects_non_object()
  {
    vix::json::Json json = vix::json::Json::array();

    auto result = kordex::runtime::Manifest::from_json(
        json,
        "package.json");

    return expect_true(
        !result,
        "non-object manifest should fail");
  }

  bool test_load_manifest_from_file()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "package.json",
        R"({
          "name": "file-app",
          "version": "0.2.0",
          "main": "index.js",
          "scripts": {
            "start": "kordex run index.js"
          }
        })");

    auto result = kordex::runtime::Manifest::load(path.string());

    cleanup_test_files();

    if (!result)
    {
      return false;
    }

    const auto &manifest = result.value();

    return expect_true(
               manifest.is_package_json(),
               "loaded manifest should be package.json") &&
           expect_true(
               manifest.name == "file-app",
               "loaded manifest name should be parsed") &&
           expect_true(
               manifest.version == "0.2.0",
               "loaded manifest version should be parsed") &&
           expect_true(
               manifest.entry == "index.js",
               "loaded manifest entry should be parsed") &&
           expect_true(
               manifest.scripts.size() == 1,
               "loaded manifest scripts should be parsed");
  }

  bool test_load_missing_manifest()
  {
    cleanup_test_files();

    const auto path = test_dir() / "missing-package.json";

    auto result = kordex::runtime::Manifest::load(path.string());

    return expect_true(
        !result,
        "missing manifest should fail");
  }

  bool test_load_invalid_json()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "kordex.json",
        "{ invalid json");

    auto result = kordex::runtime::Manifest::load(path.string());

    cleanup_test_files();

    return expect_true(
        !result,
        "invalid JSON manifest should fail");
  }

  bool run_tests()
  {
    cleanup_test_files();

    const bool ok =
        test_detect_manifest_type() &&
        test_manifest_type_to_string() &&
        test_from_json_package_manifest() &&
        test_from_json_kordex_manifest() &&
        test_from_json_rejects_non_object() &&
        test_load_manifest_from_file() &&
        test_load_missing_manifest() &&
        test_load_invalid_json();

    cleanup_test_files();

    return ok;
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
