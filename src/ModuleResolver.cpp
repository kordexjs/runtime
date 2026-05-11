/**
 *
 *  @file ModuleResolver.cpp
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

#include <vix/fs/Exists.hpp>
#include <vix/fs/IsDirectory.hpp>
#include <vix/fs/IsFile.hpp>
#include <vix/path/IsAbsolute.hpp>
#include <vix/path/Join.hpp>
#include <vix/path/Normalize.hpp>

#include <kordex/runtime/ModuleResolver.hpp>

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

    [[nodiscard]] Result<std::string> join_and_normalize(
        const std::string &base,
        const std::string &path)
    {
      auto joined = vix::path::join(base, path);
      if (!joined)
      {
        return make_runtime_error(
            RuntimeErrorCode::ModuleResolutionFailed,
            std::string(joined.error().message()));
      }

      auto normalized = vix::path::normalize(joined.value());
      if (!normalized)
      {
        return make_runtime_error(
            RuntimeErrorCode::ModuleResolutionFailed,
            std::string(normalized.error().message()));
      }

      return normalized.value();
    }

    [[nodiscard]] bool file_exists(
        const std::string &path)
    {
      auto is_file = vix::fs::is_file(path);
      return is_file && is_file.value();
    }

    [[nodiscard]] bool directory_exists(
        const std::string &path)
    {
      auto is_directory = vix::fs::is_directory(path);
      return is_directory && is_directory.value();
    }

    [[nodiscard]] Result<std::string> try_resolve_file_path(
        const std::string &candidate,
        const ModuleResolverOptions &options)
    {
      if (file_exists(candidate))
      {
        return candidate;
      }

      if (options.resolve_extensions)
      {
        for (const auto &extension : options.extensions)
        {
          const std::string with_extension = candidate + extension;

          if (file_exists(with_extension))
          {
            return with_extension;
          }
        }
      }

      if (options.resolve_index && directory_exists(candidate))
      {
        for (const auto &extension : options.extensions)
        {
          auto index_candidate = vix::path::join(
              candidate,
              "index" + extension);

          if (index_candidate && file_exists(index_candidate.value()))
          {
            return index_candidate.value();
          }
        }
      }

      return make_runtime_error(
          RuntimeErrorCode::ModuleResolutionFailed,
          "module file was not found: " + candidate);
    }
  } // namespace

  bool ResolvedModule::valid() const noexcept
  {
    return kind != ResolvedModuleKind::Unknown &&
           id.valid();
  }

  bool ResolvedModule::is_file() const noexcept
  {
    return kind == ResolvedModuleKind::File;
  }

  bool ResolvedModule::is_package() const noexcept
  {
    return kind == ResolvedModuleKind::Package;
  }

  bool ResolvedModule::is_builtin() const noexcept
  {
    return kind == ResolvedModuleKind::Builtin;
  }

  Error ResolvedModule::validate() const
  {
    if (!id.valid())
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidArgument,
          "resolved module id is invalid");
    }

    if (kind == ResolvedModuleKind::Unknown)
    {
      return make_runtime_error(
          RuntimeErrorCode::ModuleResolutionFailed,
          "resolved module kind is unknown");
    }

    if (kind == ResolvedModuleKind::File && path.empty())
    {
      return make_runtime_error(
          RuntimeErrorCode::ModuleResolutionFailed,
          "resolved file module path cannot be empty");
    }

    if (kind == ResolvedModuleKind::Package && package_name.empty())
    {
      return make_runtime_error(
          RuntimeErrorCode::ModuleResolutionFailed,
          "resolved package module name cannot be empty");
    }

    if (kind == ResolvedModuleKind::Builtin && builtin_name.empty())
    {
      return make_runtime_error(
          RuntimeErrorCode::ModuleResolutionFailed,
          "resolved builtin module name cannot be empty");
    }

    return ok();
  }

  ModuleResolver::ModuleResolver()
      : options_()
  {
  }

  ModuleResolver::ModuleResolver(
      ModuleResolverOptions options)
      : options_(std::move(options))
  {
  }

  Result<ResolvedModule> ModuleResolver::resolve(
      const std::string &specifier) const
  {
    auto id = ModuleId::parse(specifier);
    if (!id)
    {
      return id.error();
    }

    return resolve(id.value());
  }

  Result<ResolvedModule> ModuleResolver::resolve(
      const ModuleId &id) const
  {
    const auto validation = id.validate();
    if (validation)
    {
      return validation;
    }

    if (id.is_builtin())
    {
      return resolve_builtin_module(id);
    }

    if (id.is_package())
    {
      return resolve_package_module(id);
    }

    return resolve_file_module(id);
  }

  const ModuleResolverOptions &ModuleResolver::options() const noexcept
  {
    return options_;
  }

  void ModuleResolver::set_options(
      ModuleResolverOptions options)
  {
    options_ = std::move(options);
  }

  Result<ResolvedModule> ModuleResolver::resolve_file_module(
      const ModuleId &id) const
  {
    std::string candidate;

    if (id.is_absolute())
    {
      candidate = normalize_path_or_original(id.specifier());
    }
    else
    {
      auto joined = join_and_normalize(
          options_.base_dir,
          id.specifier());

      if (!joined)
      {
        return joined.error();
      }

      candidate = joined.value();
    }

    auto resolved_path = try_resolve_file_path(candidate, options_);
    if (!resolved_path)
    {
      return resolved_path.error();
    }

    ResolvedModule resolved;
    resolved.id = id;
    resolved.kind = ResolvedModuleKind::File;
    resolved.path = normalize_path_or_original(resolved_path.value());
    resolved.source_type = detect_source_type(resolved.path);
    resolved.resolved_at = Clock::now();

    const auto validation = resolved.validate();
    if (validation)
    {
      return validation;
    }

    return resolved;
  }

  Result<ResolvedModule> ModuleResolver::resolve_package_module(
      const ModuleId &id) const
  {
    if (!options_.allow_packages)
    {
      return make_runtime_error(
          RuntimeErrorCode::PermissionDenied,
          "package module resolution is disabled");
    }

    ResolvedModule resolved;
    resolved.id = id;
    resolved.kind = ResolvedModuleKind::Package;
    resolved.package_name = id.specifier();
    resolved.resolved_at = Clock::now();

    const auto validation = resolved.validate();
    if (validation)
    {
      return validation;
    }

    return resolved;
  }

  Result<ResolvedModule> ModuleResolver::resolve_builtin_module(
      const ModuleId &id) const
  {
    if (!options_.allow_builtins)
    {
      return make_runtime_error(
          RuntimeErrorCode::PermissionDenied,
          "builtin module resolution is disabled");
    }

    ResolvedModule resolved;
    resolved.id = id;
    resolved.kind = ResolvedModuleKind::Builtin;
    resolved.builtin_name = id.builtin_name();
    resolved.resolved_at = Clock::now();

    const auto validation = resolved.validate();
    if (validation)
    {
      return validation;
    }

    return resolved;
  }

  const char *to_string(
      ResolvedModuleKind kind) noexcept
  {
    switch (kind)
    {
    case ResolvedModuleKind::Unknown:
      return "unknown";
    case ResolvedModuleKind::File:
      return "file";
    case ResolvedModuleKind::Package:
      return "package";
    case ResolvedModuleKind::Builtin:
      return "builtin";
    }

    return "unknown";
  }

} // namespace kordex::runtime
