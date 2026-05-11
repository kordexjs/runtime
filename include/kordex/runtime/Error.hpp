/**
 *
 *  @file Error.hpp
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

#ifndef KORDEX_RUNTIME_ERROR_HPP
#define KORDEX_RUNTIME_ERROR_HPP

#include <string>
#include <utility>

#include <vix/error/Error.hpp>
#include <vix/error/ErrorCategory.hpp>
#include <vix/error/ErrorCode.hpp>
#include <vix/error/Exception.hpp>

namespace kordex::runtime
{
  /**
   * @brief Kordex runtime error alias.
   *
   * Kordex runtime uses the shared Vix structured error model.
   */
  using Error = vix::error::Error;

  /**
   * @brief Kordex runtime exception alias.
   */
  using Exception = vix::error::Exception;

  /**
   * @brief Kordex runtime error code alias.
   */
  using ErrorCode = vix::error::ErrorCode;

  /**
   * @brief Kordex runtime error category alias.
   */
  using ErrorCategory = vix::error::ErrorCategory;

  /**
   * @enum RuntimeErrorCode
   * @brief Runtime-specific semantic error codes.
   *
   * These codes describe common failures inside the Kordex runtime layer.
   * They are converted to the shared Vix ErrorCode model.
   */
  enum class RuntimeErrorCode
  {
    None = 0,
    InvalidArgument,
    InvalidState,
    InvalidConfig,
    SourceNotFound,
    SourceReadFailed,
    UnsupportedSourceType,
    ModuleResolutionFailed,
    ModuleLoadFailed,
    ManifestLoadFailed,
    ManifestParseFailed,
    PermissionDenied,
    ProcessFailed,
    Timeout,
    Cancelled,
    NotImplemented,
    InternalError
  };

  /**
   * @brief Return the default Kordex runtime error category.
   */
  [[nodiscard]] constexpr ErrorCategory runtime_error_category() noexcept
  {
    return ErrorCategory("kordex.runtime");
  }

  /**
   * @brief Convert a RuntimeErrorCode to a shared ErrorCode.
   */
  [[nodiscard]] constexpr ErrorCode to_error_code(RuntimeErrorCode code) noexcept
  {
    switch (code)
    {
    case RuntimeErrorCode::None:
      return ErrorCode::Ok;

    case RuntimeErrorCode::InvalidArgument:
      return ErrorCode::InvalidArgument;

    case RuntimeErrorCode::InvalidState:
      return ErrorCode::InvalidState;

    case RuntimeErrorCode::InvalidConfig:
      return ErrorCode::ConfigError;

    case RuntimeErrorCode::SourceNotFound:
      return ErrorCode::NotFound;

    case RuntimeErrorCode::SourceReadFailed:
      return ErrorCode::IoError;

    case RuntimeErrorCode::UnsupportedSourceType:
      return ErrorCode::NotSupported;

    case RuntimeErrorCode::ModuleResolutionFailed:
    case RuntimeErrorCode::ModuleLoadFailed:
      return ErrorCode::InternalError;

    case RuntimeErrorCode::ManifestLoadFailed:
      return ErrorCode::IoError;

    case RuntimeErrorCode::ManifestParseFailed:
      return ErrorCode::ParseError;

    case RuntimeErrorCode::PermissionDenied:
      return ErrorCode::PermissionDenied;

    case RuntimeErrorCode::ProcessFailed:
      return ErrorCode::ExternalError;

    case RuntimeErrorCode::Timeout:
      return ErrorCode::Timeout;

    case RuntimeErrorCode::Cancelled:
      return ErrorCode::Cancelled;

    case RuntimeErrorCode::NotImplemented:
      return ErrorCode::NotImplemented;

    case RuntimeErrorCode::InternalError:
      return ErrorCode::InternalError;
    }

    return ErrorCode::Unknown;
  }

  /**
   * @brief Convert a RuntimeErrorCode to a stable string.
   */
  [[nodiscard]] constexpr const char *to_string(RuntimeErrorCode code) noexcept
  {
    switch (code)
    {
    case RuntimeErrorCode::None:
      return "none";
    case RuntimeErrorCode::InvalidArgument:
      return "invalid_argument";
    case RuntimeErrorCode::InvalidState:
      return "invalid_state";
    case RuntimeErrorCode::InvalidConfig:
      return "invalid_config";
    case RuntimeErrorCode::SourceNotFound:
      return "source_not_found";
    case RuntimeErrorCode::SourceReadFailed:
      return "source_read_failed";
    case RuntimeErrorCode::UnsupportedSourceType:
      return "unsupported_source_type";
    case RuntimeErrorCode::ModuleResolutionFailed:
      return "module_resolution_failed";
    case RuntimeErrorCode::ModuleLoadFailed:
      return "module_load_failed";
    case RuntimeErrorCode::ManifestLoadFailed:
      return "manifest_load_failed";
    case RuntimeErrorCode::ManifestParseFailed:
      return "manifest_parse_failed";
    case RuntimeErrorCode::PermissionDenied:
      return "permission_denied";
    case RuntimeErrorCode::ProcessFailed:
      return "process_failed";
    case RuntimeErrorCode::Timeout:
      return "timeout";
    case RuntimeErrorCode::Cancelled:
      return "cancelled";
    case RuntimeErrorCode::NotImplemented:
      return "not_implemented";
    case RuntimeErrorCode::InternalError:
      return "internal_error";
    }

    return "unknown";
  }

  /**
   * @brief Build a structured runtime error.
   */
  [[nodiscard]] inline Error make_runtime_error(
      RuntimeErrorCode code,
      std::string message)
  {
    return Error(
        to_error_code(code),
        runtime_error_category(),
        std::move(message));
  }

  /**
   * @brief Return a success error object.
   */
  [[nodiscard]] inline Error ok() noexcept
  {
    return {};
  }

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_ERROR_HPP
