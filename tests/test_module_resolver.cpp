/**
 *
 *  @file test_module_resolver.cpp
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

#include <kordex/runtime/ModuleResolver.hpp>

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
           "kordex_runtime_module_resolver_tests";
  }

  std::filesystem::path write_test_file(
      const std::string &relative_path,
      const std::string &content)
  {
    const auto path = test_dir() / relative_path;

    std::filesystem::create_directories(path.parent_path());

    std::ofstream out(path, std::ios::trunc);
    out << content;

    return path;
  }

  void cleanup_test_files()
  {
    std::error_code ec;
    std::filesystem::remove_all(test_dir(), ec);
  }

  kordex::runtime::ModuleResolver make_resolver()
  {
    kordex::runtime::ModuleResolverOptions options;
    options.base_dir = test_dir().string();
    options.allow_builtins = true;
    options.allow_packages = true;
    options.resolve_extensions = true;
    options.resolve_index = true;

    return kordex::runtime::ModuleResolver(options);
  }

  bool test_resolved_module_kind_to_string()
  {
    using kordex::runtime::ResolvedModuleKind;

    return expect_true(
               std::string_view(kordex::runtime::to_string(ResolvedModuleKind::Unknown)) == "unknown",
               "unknown should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(ResolvedModuleKind::File)) == "file",
               "file should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(ResolvedModuleKind::Package)) == "package",
               "package should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(ResolvedModuleKind::Builtin)) == "builtin",
               "builtin should convert to string");
  }

  bool test_default_resolver_options()
  {
    kordex::runtime::ModuleResolver resolver;
    const auto &options = resolver.options();

    return expect_true(
               options.base_dir == ".",
               "default base directory should be current directory") &&
           expect_true(
               options.allow_builtins,
               "builtins should be allowed by default") &&
           expect_true(
               options.allow_packages,
               "packages should be allowed by default") &&
           expect_true(
               options.resolve_extensions,
               "extension resolution should be enabled by default") &&
           expect_true(
               options.resolve_index,
               "index resolution should be enabled by default") &&
           expect_true(
               !options.extensions.empty(),
               "default extensions should not be empty");
  }

  bool test_set_options()
  {
    kordex::runtime::ModuleResolver resolver;

    kordex::runtime::ModuleResolverOptions options;
    options.base_dir = "/tmp/kordex";
    options.allow_builtins = false;
    options.allow_packages = false;
    options.resolve_extensions = false;
    options.resolve_index = false;

    resolver.set_options(options);

    const auto &current = resolver.options();

    return expect_true(
               current.base_dir == "/tmp/kordex",
               "set_options should update base_dir") &&
           expect_true(
               !current.allow_builtins,
               "set_options should update allow_builtins") &&
           expect_true(
               !current.allow_packages,
               "set_options should update allow_packages") &&
           expect_true(
               !current.resolve_extensions,
               "set_options should update resolve_extensions") &&
           expect_true(
               !current.resolve_index,
               "set_options should update resolve_index");
  }

  bool test_resolve_relative_file_with_extension()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "src/app.js",
        "console.log('hello');");

    auto resolver = make_resolver();
    auto result = resolver.resolve("./src/app.js");

    cleanup_test_files();

    if (!result)
    {
      return false;
    }

    const auto &resolved = result.value();

    return expect_true(
               resolved.valid(),
               "resolved file module should be valid") &&
           expect_true(
               resolved.is_file(),
               "resolved module should be file") &&
           expect_true(
               resolved.kind == kordex::runtime::ResolvedModuleKind::File,
               "resolved kind should be File") &&
           expect_true(
               resolved.path == path.string(),
               "resolved path should match file path") &&
           expect_true(
               resolved.source_type == kordex::runtime::SourceType::JavaScript,
               "resolved source type should be JavaScript") &&
           expect_true(
               !resolved.resolved_at.is_zero(),
               "resolved_at should be set");
  }

  bool test_resolve_relative_file_without_extension()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "src/app.ts",
        "const value: number = 1;");

    auto resolver = make_resolver();
    auto result = resolver.resolve("./src/app");

    cleanup_test_files();

    if (!result)
    {
      return false;
    }

    const auto &resolved = result.value();

    return expect_true(
               resolved.is_file(),
               "resolved module should be file") &&
           expect_true(
               resolved.path == path.string(),
               "resolver should find file by extension") &&
           expect_true(
               resolved.source_type == kordex::runtime::SourceType::TypeScript,
               "resolved source type should be TypeScript");
  }

  bool test_resolve_directory_index()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "lib/index.js",
        "export const value = 1;");

    auto resolver = make_resolver();
    auto result = resolver.resolve("./lib");

    cleanup_test_files();

    if (!result)
    {
      return false;
    }

    const auto &resolved = result.value();

    return expect_true(
               resolved.is_file(),
               "directory import should resolve to file") &&
           expect_true(
               resolved.path == path.string(),
               "directory import should resolve to index.js") &&
           expect_true(
               resolved.source_type == kordex::runtime::SourceType::JavaScript,
               "resolved source type should be JavaScript");
  }

  bool test_resolve_absolute_file()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "absolute/main.mjs",
        "console.log('absolute');");

    auto resolver = make_resolver();
    auto result = resolver.resolve(path.string());

    cleanup_test_files();

    if (!result)
    {
      return false;
    }

    const auto &resolved = result.value();

    return expect_true(
               resolved.is_file(),
               "absolute import should resolve to file") &&
           expect_true(
               resolved.path == path.string(),
               "absolute import should preserve absolute path") &&
           expect_true(
               resolved.id.is_absolute(),
               "module id should report absolute") &&
           expect_true(
               resolved.source_type == kordex::runtime::SourceType::JavaScript,
               "absolute module should be JavaScript");
  }

  bool test_resolve_json_file()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "package.json",
        R"({"name":"demo"})");

    auto resolver = make_resolver();
    auto result = resolver.resolve("./package.json");

    cleanup_test_files();

    if (!result)
    {
      return false;
    }

    const auto &resolved = result.value();

    return expect_true(
               resolved.is_file(),
               "json import should resolve to file") &&
           expect_true(
               resolved.path == path.string(),
               "json path should match") &&
           expect_true(
               resolved.source_type == kordex::runtime::SourceType::Json,
               "resolved source type should be JSON");
  }

  bool test_resolve_builtin_module()
  {
    auto resolver = make_resolver();
    auto result = resolver.resolve("kordex:fs");

    if (!result)
    {
      return false;
    }

    const auto &resolved = result.value();

    return expect_true(
               resolved.valid(),
               "builtin resolved module should be valid") &&
           expect_true(
               resolved.is_builtin(),
               "resolved module should be builtin") &&
           expect_true(
               resolved.kind == kordex::runtime::ResolvedModuleKind::Builtin,
               "resolved kind should be Builtin") &&
           expect_true(
               resolved.builtin_name == "fs",
               "builtin name should be fs") &&
           expect_true(
               resolved.path.empty(),
               "builtin module should not have file path") &&
           expect_true(
               !resolved.resolved_at.is_zero(),
               "builtin resolved_at should be set");
  }

  bool test_resolve_package_module()
  {
    auto resolver = make_resolver();
    auto result = resolver.resolve("@scope/pkg");

    if (!result)
    {
      return false;
    }

    const auto &resolved = result.value();

    return expect_true(
               resolved.valid(),
               "package resolved module should be valid") &&
           expect_true(
               resolved.is_package(),
               "resolved module should be package") &&
           expect_true(
               resolved.kind == kordex::runtime::ResolvedModuleKind::Package,
               "resolved kind should be Package") &&
           expect_true(
               resolved.package_name == "@scope/pkg",
               "package name should be preserved") &&
           expect_true(
               resolved.path.empty(),
               "package module should not have file path yet") &&
           expect_true(
               !resolved.resolved_at.is_zero(),
               "package resolved_at should be set");
  }

  bool test_reject_builtin_when_disabled()
  {
    kordex::runtime::ModuleResolverOptions options;
    options.base_dir = test_dir().string();
    options.allow_builtins = false;

    kordex::runtime::ModuleResolver resolver(options);

    auto result = resolver.resolve("kordex:fs");

    return expect_true(
               !result,
               "builtin resolution should fail when disabled") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::PermissionDenied,
               "disabled builtin resolution should use PermissionDenied");
  }

  bool test_reject_package_when_disabled()
  {
    kordex::runtime::ModuleResolverOptions options;
    options.base_dir = test_dir().string();
    options.allow_packages = false;

    kordex::runtime::ModuleResolver resolver(options);

    auto result = resolver.resolve("react");

    return expect_true(
               !result,
               "package resolution should fail when disabled") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::PermissionDenied,
               "disabled package resolution should use PermissionDenied");
  }

  bool test_reject_missing_file()
  {
    cleanup_test_files();

    auto resolver = make_resolver();
    auto result = resolver.resolve("./missing.js");

    cleanup_test_files();

    return expect_true(
               !result,
               "missing file should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::InternalError,
               "missing file should use ModuleResolutionFailed mapped code");
  }

  bool test_reject_invalid_specifier()
  {
    auto resolver = make_resolver();
    auto result = resolver.resolve("");

    return expect_true(
               !result,
               "empty specifier should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::InvalidArgument,
               "empty specifier should use InvalidArgument");
  }

  bool test_resolve_parsed_module_id()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "src/parsed.js",
        "console.log('parsed');");

    auto id = kordex::runtime::ModuleId::parse("./src/parsed.js");
    if (!id)
    {
      cleanup_test_files();
      return false;
    }

    auto resolver = make_resolver();
    auto result = resolver.resolve(id.value());

    cleanup_test_files();

    if (!result)
    {
      return false;
    }

    const auto &resolved = result.value();

    return expect_true(
               resolved.is_file(),
               "parsed module id should resolve to file") &&
           expect_true(
               resolved.path == path.string(),
               "parsed module id should resolve to expected path");
  }

  bool test_resolved_module_validation_rejects_invalid_id()
  {
    kordex::runtime::ResolvedModule resolved;
    resolved.kind = kordex::runtime::ResolvedModuleKind::File;
    resolved.path = "app.js";

    const auto error = resolved.validate();

    return expect_true(
               error.has_error(),
               "resolved module with invalid id should fail") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::InvalidArgument,
               "invalid resolved module id should use InvalidArgument");
  }

  bool test_resolved_module_validation_rejects_unknown_kind()
  {
    auto id = kordex::runtime::ModuleId::parse("./app.js");
    if (!id)
    {
      return false;
    }

    kordex::runtime::ResolvedModule resolved;
    resolved.id = id.value();
    resolved.kind = kordex::runtime::ResolvedModuleKind::Unknown;

    const auto error = resolved.validate();

    return expect_true(
               error.has_error(),
               "resolved module with unknown kind should fail") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::InternalError,
               "unknown resolved module kind should use InternalError");
  }

  bool test_resolved_module_validation_rejects_empty_file_path()
  {
    auto id = kordex::runtime::ModuleId::parse("./app.js");
    if (!id)
    {
      return false;
    }

    kordex::runtime::ResolvedModule resolved;
    resolved.id = id.value();
    resolved.kind = kordex::runtime::ResolvedModuleKind::File;

    const auto error = resolved.validate();

    return expect_true(
               error.has_error(),
               "resolved file module without path should fail") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::InternalError,
               "empty resolved file path should use InternalError");
  }

  bool run_tests()
  {
    cleanup_test_files();

    const bool ok =
        test_resolved_module_kind_to_string() &&
        test_default_resolver_options() &&
        test_set_options() &&
        test_resolve_relative_file_with_extension() &&
        test_resolve_relative_file_without_extension() &&
        test_resolve_directory_index() &&
        test_resolve_absolute_file() &&
        test_resolve_json_file() &&
        test_resolve_builtin_module() &&
        test_resolve_package_module() &&
        test_reject_builtin_when_disabled() &&
        test_reject_package_when_disabled() &&
        test_reject_missing_file() &&
        test_reject_invalid_specifier() &&
        test_resolve_parsed_module_id() &&
        test_resolved_module_validation_rejects_invalid_id() &&
        test_resolved_module_validation_rejects_unknown_kind() &&
        test_resolved_module_validation_rejects_empty_file_path();

    cleanup_test_files();

    return ok;
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
