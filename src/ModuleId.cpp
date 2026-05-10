/**
 *
 *  @file ModuleId.cpp
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

#include <utility>
#include <vix/path/IsAbsolute.hpp>
#include <kordex/runtime/ModuleId.hpp>

namespace kordex::runtime
{
  namespace
  {
    [[nodiscard]] bool starts_with(
        std::string_view value,
        std::string_view prefix) noexcept
    {
      return value.size() >= prefix.size() &&
             value.substr(0, prefix.size()) == prefix;
    }

    [[nodiscard]] bool contains_null_byte(
        std::string_view value) noexcept
    {
      return value.find('\0') != std::string_view::npos;
    }
  } // namespace

  ModuleId::ModuleId(
      ModuleIdKind kind,
      std::string specifier)
      : kind_(kind),
        specifier_(std::move(specifier))
  {
  }

  Result<ModuleId> ModuleId::parse(
      std::string specifier)
  {
    ModuleId module_id(
        detect_module_id_kind(specifier),
        std::move(specifier));

    const auto validation = module_id.validate();
    if (validation)
    {
      return validation;
    }

    return module_id;
  }

  ModuleIdKind ModuleId::kind() const noexcept
  {
    return kind_;
  }

  const std::string &ModuleId::specifier() const noexcept
  {
    return specifier_;
  }

  bool ModuleId::valid() const noexcept
  {
    return kind_ != ModuleIdKind::Unknown &&
           !specifier_.empty();
  }

  bool ModuleId::is_relative() const noexcept
  {
    return kind_ == ModuleIdKind::Relative;
  }

  bool ModuleId::is_absolute() const noexcept
  {
    return kind_ == ModuleIdKind::Absolute;
  }

  bool ModuleId::is_package() const noexcept
  {
    return kind_ == ModuleIdKind::Package;
  }

  bool ModuleId::is_builtin() const noexcept
  {
    return kind_ == ModuleIdKind::Builtin;
  }

  bool ModuleId::needs_filesystem_resolution() const noexcept
  {
    return is_relative() || is_absolute();
  }

  std::string ModuleId::builtin_name() const
  {
    if (!is_builtin())
    {
      return {};
    }

    constexpr std::string_view prefix = "kordex:";

    if (!starts_with(specifier_, prefix))
    {
      return {};
    }

    return std::string(specifier_.substr(prefix.size()));
  }

  Error ModuleId::validate() const
  {
    if (specifier_.empty())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "module specifier cannot be empty");
    }

    if (contains_null_byte(specifier_))
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "module specifier cannot contain null bytes");
    }

    if (kind_ == ModuleIdKind::Unknown)
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "module specifier is invalid: " + specifier_);
    }

    if (kind_ == ModuleIdKind::Builtin && builtin_name().empty())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "builtin module name cannot be empty");
    }

    return ok();
  }

  ModuleIdKind detect_module_id_kind(
      std::string_view specifier) noexcept
  {
    if (specifier.empty())
    {
      return ModuleIdKind::Unknown;
    }

    if (contains_null_byte(specifier))
    {
      return ModuleIdKind::Unknown;
    }

    if (starts_with(specifier, "kordex:"))
    {
      return specifier.size() > std::string_view("kordex:").size()
                 ? ModuleIdKind::Builtin
                 : ModuleIdKind::Unknown;
    }

    if (starts_with(specifier, "./") ||
        starts_with(specifier, "../") ||
        specifier == "." ||
        specifier == "..")
    {
      return ModuleIdKind::Relative;
    }

    if (vix::path::is_absolute(specifier))
    {
      return ModuleIdKind::Absolute;
    }

    if (specifier.front() == '/' || specifier.front() == '\\')
    {
      return ModuleIdKind::Absolute;
    }

    return ModuleIdKind::Package;
  }

  const char *to_string(
      ModuleIdKind kind) noexcept
  {
    switch (kind)
    {
    case ModuleIdKind::Unknown:
      return "unknown";
    case ModuleIdKind::Relative:
      return "relative";
    case ModuleIdKind::Absolute:
      return "absolute";
    case ModuleIdKind::Package:
      return "package";
    case ModuleIdKind::Builtin:
      return "builtin";
    }

    return "unknown";
  }

} // namespace kordex::runtime
