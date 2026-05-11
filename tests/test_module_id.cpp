/**
 *
 *  @file test_module_id.cpp
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

#include <string_view>
#include <kordex/runtime/ModuleId.hpp>

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

  bool test_detect_module_id_kind()
  {
    using kordex::runtime::ModuleIdKind;

    return expect_true(
               kordex::runtime::detect_module_id_kind("") == ModuleIdKind::Unknown,
               "empty specifier should be unknown") &&
           expect_true(
               kordex::runtime::detect_module_id_kind("./app.js") == ModuleIdKind::Relative,
               "./app.js should be relative") &&
           expect_true(
               kordex::runtime::detect_module_id_kind("../lib/mod.js") == ModuleIdKind::Relative,
               "../lib/mod.js should be relative") &&
           expect_true(
               kordex::runtime::detect_module_id_kind(".") == ModuleIdKind::Relative,
               ". should be relative") &&
           expect_true(
               kordex::runtime::detect_module_id_kind("..") == ModuleIdKind::Relative,
               ".. should be relative") &&
           expect_true(
               kordex::runtime::detect_module_id_kind("/app/main.js") == ModuleIdKind::Absolute,
               "/app/main.js should be absolute") &&
           expect_true(
               kordex::runtime::detect_module_id_kind("\\app\\main.js") == ModuleIdKind::Absolute,
               "\\app\\main.js should be absolute") &&
           expect_true(
               kordex::runtime::detect_module_id_kind("kordex:fs") == ModuleIdKind::Builtin,
               "kordex:fs should be builtin") &&
           expect_true(
               kordex::runtime::detect_module_id_kind("kordex:") == ModuleIdKind::Unknown,
               "empty builtin name should be unknown") &&
           expect_true(
               kordex::runtime::detect_module_id_kind("react") == ModuleIdKind::Package,
               "react should be package") &&
           expect_true(
               kordex::runtime::detect_module_id_kind("@scope/pkg") == ModuleIdKind::Package,
               "@scope/pkg should be package");
  }

  bool test_module_id_kind_to_string()
  {
    using kordex::runtime::ModuleIdKind;

    return expect_true(
               std::string_view(kordex::runtime::to_string(ModuleIdKind::Unknown)) == "unknown",
               "unknown should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(ModuleIdKind::Relative)) == "relative",
               "relative should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(ModuleIdKind::Absolute)) == "absolute",
               "absolute should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(ModuleIdKind::Package)) == "package",
               "package should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(ModuleIdKind::Builtin)) == "builtin",
               "builtin should convert to string");
  }

  bool test_parse_relative_module_id()
  {
    auto result = kordex::runtime::ModuleId::parse("./app.js");

    if (!result)
    {
      return false;
    }

    const auto &id = result.value();

    return expect_true(
               id.valid(),
               "relative module id should be valid") &&
           expect_true(
               id.kind() == kordex::runtime::ModuleIdKind::Relative,
               "relative module id kind should be Relative") &&
           expect_true(
               id.specifier() == "./app.js",
               "relative module id should preserve specifier") &&
           expect_true(
               id.is_relative(),
               "relative module id should report relative") &&
           expect_true(
               !id.is_absolute(),
               "relative module id should not report absolute") &&
           expect_true(
               !id.is_package(),
               "relative module id should not report package") &&
           expect_true(
               !id.is_builtin(),
               "relative module id should not report builtin") &&
           expect_true(
               id.needs_filesystem_resolution(),
               "relative module id should need filesystem resolution");
  }

  bool test_parse_absolute_module_id()
  {
    auto result = kordex::runtime::ModuleId::parse("/app/main.js");

    if (!result)
    {
      return false;
    }

    const auto &id = result.value();

    return expect_true(
               id.valid(),
               "absolute module id should be valid") &&
           expect_true(
               id.kind() == kordex::runtime::ModuleIdKind::Absolute,
               "absolute module id kind should be Absolute") &&
           expect_true(
               id.specifier() == "/app/main.js",
               "absolute module id should preserve specifier") &&
           expect_true(
               id.is_absolute(),
               "absolute module id should report absolute") &&
           expect_true(
               !id.is_relative(),
               "absolute module id should not report relative") &&
           expect_true(
               !id.is_package(),
               "absolute module id should not report package") &&
           expect_true(
               !id.is_builtin(),
               "absolute module id should not report builtin") &&
           expect_true(
               id.needs_filesystem_resolution(),
               "absolute module id should need filesystem resolution");
  }

  bool test_parse_package_module_id()
  {
    auto result = kordex::runtime::ModuleId::parse("@scope/pkg");

    if (!result)
    {
      return false;
    }

    const auto &id = result.value();

    return expect_true(
               id.valid(),
               "package module id should be valid") &&
           expect_true(
               id.kind() == kordex::runtime::ModuleIdKind::Package,
               "package module id kind should be Package") &&
           expect_true(
               id.specifier() == "@scope/pkg",
               "package module id should preserve specifier") &&
           expect_true(
               id.is_package(),
               "package module id should report package") &&
           expect_true(
               !id.is_relative(),
               "package module id should not report relative") &&
           expect_true(
               !id.is_absolute(),
               "package module id should not report absolute") &&
           expect_true(
               !id.is_builtin(),
               "package module id should not report builtin") &&
           expect_true(
               !id.needs_filesystem_resolution(),
               "package module id should not need filesystem resolution");
  }

  bool test_parse_builtin_module_id()
  {
    auto result = kordex::runtime::ModuleId::parse("kordex:fs");

    if (!result)
    {
      return false;
    }

    const auto &id = result.value();

    return expect_true(
               id.valid(),
               "builtin module id should be valid") &&
           expect_true(
               id.kind() == kordex::runtime::ModuleIdKind::Builtin,
               "builtin module id kind should be Builtin") &&
           expect_true(
               id.specifier() == "kordex:fs",
               "builtin module id should preserve specifier") &&
           expect_true(
               id.is_builtin(),
               "builtin module id should report builtin") &&
           expect_true(
               !id.is_relative(),
               "builtin module id should not report relative") &&
           expect_true(
               !id.is_absolute(),
               "builtin module id should not report absolute") &&
           expect_true(
               !id.is_package(),
               "builtin module id should not report package") &&
           expect_true(
               !id.needs_filesystem_resolution(),
               "builtin module id should not need filesystem resolution") &&
           expect_true(
               id.builtin_name() == "fs",
               "builtin module id should expose builtin name");
  }

  bool test_parse_rejects_empty_specifier()
  {
    auto result = kordex::runtime::ModuleId::parse("");

    return expect_true(
               !result,
               "empty module specifier should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::InvalidArgument,
               "empty module specifier should use InvalidArgument");
  }

  bool test_parse_rejects_empty_builtin_name()
  {
    auto result = kordex::runtime::ModuleId::parse("kordex:");

    return expect_true(
               !result,
               "empty builtin name should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::InvalidArgument,
               "empty builtin name should use InvalidArgument");
  }

  bool test_parse_rejects_null_byte()
  {
    std::string specifier = "app";
    specifier.push_back('\0');
    specifier += "js";

    auto result = kordex::runtime::ModuleId::parse(specifier);

    return expect_true(
               !result,
               "module specifier with null byte should fail") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::InvalidArgument,
               "null byte module specifier should use InvalidArgument");
  }

  bool test_builtin_name_returns_empty_for_non_builtin()
  {
    auto result = kordex::runtime::ModuleId::parse("./app.js");

    if (!result)
    {
      return false;
    }

    return expect_true(
        result.value().builtin_name().empty(),
        "non-builtin module id should return empty builtin name");
  }

  bool test_validate_invalid_default_module_id()
  {
    kordex::runtime::ModuleId id;

    const auto error = id.validate();

    return expect_true(
               error.has_error(),
               "default module id should be invalid") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::InvalidArgument,
               "default module id validation should use InvalidArgument");
  }

  bool test_manual_module_id_constructor()
  {
    kordex::runtime::ModuleId id(
        kordex::runtime::ModuleIdKind::Package,
        "lodash");

    return expect_true(
               id.valid(),
               "manually constructed package module id should be valid") &&
           expect_true(
               id.kind() == kordex::runtime::ModuleIdKind::Package,
               "manual module id kind should be Package") &&
           expect_true(
               id.specifier() == "lodash",
               "manual module id should preserve specifier") &&
           expect_true(
               !id.validate().has_error(),
               "manual module id should validate");
  }

  bool run_tests()
  {
    return test_detect_module_id_kind() &&
           test_module_id_kind_to_string() &&
           test_parse_relative_module_id() &&
           test_parse_absolute_module_id() &&
           test_parse_package_module_id() &&
           test_parse_builtin_module_id() &&
           test_parse_rejects_empty_specifier() &&
           test_parse_rejects_empty_builtin_name() &&
           test_parse_rejects_null_byte() &&
           test_builtin_name_returns_empty_for_non_builtin() &&
           test_validate_invalid_default_module_id() &&
           test_manual_module_id_constructor();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
