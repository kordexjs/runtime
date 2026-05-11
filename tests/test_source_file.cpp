/**
 *
 *  @file test_source_file.cpp
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

#include <filesystem>
#include <fstream>
#include <string_view>

#include <kordex/runtime/SourceFile.hpp>

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
           "kordex_runtime_source_file_tests";
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

  bool test_detect_source_type()
  {
    using kordex::runtime::SourceType;

    return expect_true(
               kordex::runtime::detect_source_type("app.js") == SourceType::JavaScript,
               ".js should be detected as JavaScript") &&
           expect_true(
               kordex::runtime::detect_source_type("app.mjs") == SourceType::JavaScript,
               ".mjs should be detected as JavaScript") &&
           expect_true(
               kordex::runtime::detect_source_type("app.cjs") == SourceType::JavaScript,
               ".cjs should be detected as JavaScript") &&
           expect_true(
               kordex::runtime::detect_source_type("app.ts") == SourceType::TypeScript,
               ".ts should be detected as TypeScript") &&
           expect_true(
               kordex::runtime::detect_source_type("app.mts") == SourceType::TypeScript,
               ".mts should be detected as TypeScript") &&
           expect_true(
               kordex::runtime::detect_source_type("app.cts") == SourceType::TypeScript,
               ".cts should be detected as TypeScript") &&
           expect_true(
               kordex::runtime::detect_source_type("package.json") == SourceType::Json,
               ".json should be detected as JSON") &&
           expect_true(
               kordex::runtime::detect_source_type("README.md") == SourceType::Unknown,
               "unknown extension should be detected as Unknown");
  }

  bool test_source_type_to_string()
  {
    using kordex::runtime::SourceType;

    return expect_true(
               std::string_view(kordex::runtime::to_string(SourceType::Unknown)) == "unknown",
               "unknown should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(SourceType::JavaScript)) == "javascript",
               "javascript should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(SourceType::TypeScript)) == "typescript",
               "typescript should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(SourceType::Json)) == "json",
               "json should convert to string");
  }

  bool test_from_content_javascript()
  {
    auto result = kordex::runtime::SourceFile::from_content(
        "src/app.js",
        "console.log('hello');");

    if (!result)
    {
      return false;
    }

    const auto &source = result.value();

    return expect_true(
               source.original_path == "src/app.js",
               "original path should be preserved") &&
           expect_true(
               source.path == "src/app.js",
               "normalized path should be preserved for simple path") &&
           expect_true(
               source.type == kordex::runtime::SourceType::JavaScript,
               "source type should be JavaScript") &&
           expect_true(
               source.content == "console.log('hello');",
               "source content should be preserved") &&
           expect_true(
               !source.loaded_at.is_zero(),
               "loaded_at should be set") &&
           expect_true(
               !source.empty(),
               "source should not be empty") &&
           expect_true(
               source.size() == 21,
               "source size should match content size") &&
           expect_true(
               source.is_javascript(),
               "source should report JavaScript") &&
           expect_true(
               !source.is_typescript(),
               "source should not report TypeScript") &&
           expect_true(
               !source.is_json(),
               "source should not report JSON") &&
           expect_true(
               source.executable(),
               "JavaScript source should be executable");
  }

  bool test_from_content_typescript()
  {
    auto result = kordex::runtime::SourceFile::from_content(
        "src/app.ts",
        "const value: number = 1;");

    if (!result)
    {
      return false;
    }

    const auto &source = result.value();

    return expect_true(
               source.type == kordex::runtime::SourceType::TypeScript,
               "source type should be TypeScript") &&
           expect_true(
               source.is_typescript(),
               "source should report TypeScript") &&
           expect_true(
               source.executable(),
               "TypeScript source should be executable");
  }

  bool test_from_content_json()
  {
    auto result = kordex::runtime::SourceFile::from_content(
        "package.json",
        R"({"name":"demo"})");

    if (!result)
    {
      return false;
    }

    const auto &source = result.value();

    return expect_true(
               source.type == kordex::runtime::SourceType::Json,
               "source type should be JSON") &&
           expect_true(
               source.is_json(),
               "source should report JSON") &&
           expect_true(
               !source.executable(),
               "JSON source should not be executable");
  }

  bool test_from_content_rejects_empty_path()
  {
    auto result = kordex::runtime::SourceFile::from_content(
        "",
        "console.log('hello');");

    return expect_true(
               !result,
               "empty source path should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::InvalidArgument,
               "empty source path should use InvalidArgument");
  }

  bool test_from_content_rejects_unknown_type()
  {
    auto result = kordex::runtime::SourceFile::from_content(
        "README.md",
        "# README");

    return expect_true(
               !result,
               "unknown source type should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::NotSupported,
               "unknown source type should use NotSupported");
  }

  bool test_load_javascript_file()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "app.js",
        "console.log('from file');");

    auto result = kordex::runtime::SourceFile::load(path.string());

    cleanup_test_files();

    if (!result)
    {
      return false;
    }

    const auto &source = result.value();

    return expect_true(
               source.original_path == path.string(),
               "loaded source original path should be preserved") &&
           expect_true(
               source.type == kordex::runtime::SourceType::JavaScript,
               "loaded source type should be JavaScript") &&
           expect_true(
               source.content == "console.log('from file');",
               "loaded source content should be preserved") &&
           expect_true(
               source.is_javascript(),
               "loaded source should report JavaScript") &&
           expect_true(
               source.executable(),
               "loaded JavaScript source should be executable");
  }

  bool test_load_typescript_file()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "app.ts",
        "const value: number = 42;");

    auto result = kordex::runtime::SourceFile::load(path.string());

    cleanup_test_files();

    if (!result)
    {
      return false;
    }

    const auto &source = result.value();

    return expect_true(
               source.type == kordex::runtime::SourceType::TypeScript,
               "loaded source type should be TypeScript") &&
           expect_true(
               source.is_typescript(),
               "loaded source should report TypeScript") &&
           expect_true(
               source.executable(),
               "loaded TypeScript source should be executable");
  }

  bool test_load_json_file()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "package.json",
        R"({"name":"demo"})");

    auto result = kordex::runtime::SourceFile::load(path.string());

    cleanup_test_files();

    if (!result)
    {
      return false;
    }

    const auto &source = result.value();

    return expect_true(
               source.type == kordex::runtime::SourceType::Json,
               "loaded source type should be JSON") &&
           expect_true(
               source.is_json(),
               "loaded source should report JSON") &&
           expect_true(
               !source.executable(),
               "loaded JSON source should not be executable");
  }

  bool test_load_missing_file()
  {
    cleanup_test_files();

    const auto path = test_dir() / "missing.js";

    auto result = kordex::runtime::SourceFile::load(path.string());

    return expect_true(
               !result,
               "missing source file should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::NotFound,
               "missing source file should use NotFound");
  }

  bool test_load_rejects_empty_path()
  {
    auto result = kordex::runtime::SourceFile::load("");

    return expect_true(
               !result,
               "empty load path should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::InvalidArgument,
               "empty load path should use InvalidArgument");
  }

  bool test_load_rejects_unsupported_file()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "README.md",
        "# README");

    auto result = kordex::runtime::SourceFile::load(path.string());

    cleanup_test_files();

    return expect_true(
               !result,
               "unsupported source file should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::NotSupported,
               "unsupported source file should use NotSupported");
  }

  bool test_validate_rejects_empty_path()
  {
    kordex::runtime::SourceFile source;
    source.type = kordex::runtime::SourceType::JavaScript;
    source.content = "console.log('hello');";

    const auto error = source.validate();

    return expect_true(
               error.has_error(),
               "source validation should reject empty path") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::InvalidArgument,
               "empty path validation should use InvalidArgument");
  }

  bool test_validate_rejects_unknown_type()
  {
    kordex::runtime::SourceFile source;
    source.path = "README.md";
    source.type = kordex::runtime::SourceType::Unknown;
    source.content = "# README";

    const auto error = source.validate();

    return expect_true(
               error.has_error(),
               "source validation should reject unknown type") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::NotSupported,
               "unknown type validation should use NotSupported");
  }

  bool run_tests()
  {
    cleanup_test_files();

    const bool ok =
        test_detect_source_type() &&
        test_source_type_to_string() &&
        test_from_content_javascript() &&
        test_from_content_typescript() &&
        test_from_content_json() &&
        test_from_content_rejects_empty_path() &&
        test_from_content_rejects_unknown_type() &&
        test_load_javascript_file() &&
        test_load_typescript_file() &&
        test_load_json_file() &&
        test_load_missing_file() &&
        test_load_rejects_empty_path() &&
        test_load_rejects_unsupported_file() &&
        test_validate_rejects_empty_path() &&
        test_validate_rejects_unknown_type();

    cleanup_test_files();

    return ok;
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
