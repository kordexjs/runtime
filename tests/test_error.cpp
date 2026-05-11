/**
 *
 *  @file test_error.cpp
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

#include <kordex/runtime/Error.hpp>

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

  bool test_runtime_error_category()
  {
    return expect_true(
        kordex::runtime::runtime_error_category().name() == "kordex.runtime",
        "runtime error category should be kordex.runtime");
  }

  bool test_runtime_error_code_mapping()
  {
    using kordex::runtime::ErrorCode;
    using kordex::runtime::RuntimeErrorCode;
    using kordex::runtime::to_error_code;

    return expect_true(
               to_error_code(RuntimeErrorCode::None) == ErrorCode::Ok,
               "None should map to Ok") &&
           expect_true(
               to_error_code(RuntimeErrorCode::InvalidArgument) == ErrorCode::InvalidArgument,
               "InvalidArgument should map to InvalidArgument") &&
           expect_true(
               to_error_code(RuntimeErrorCode::InvalidState) == ErrorCode::InvalidState,
               "InvalidState should map to InvalidState") &&
           expect_true(
               to_error_code(RuntimeErrorCode::InvalidConfig) == ErrorCode::ConfigError,
               "InvalidConfig should map to ConfigError") &&
           expect_true(
               to_error_code(RuntimeErrorCode::SourceNotFound) == ErrorCode::NotFound,
               "SourceNotFound should map to NotFound") &&
           expect_true(
               to_error_code(RuntimeErrorCode::SourceReadFailed) == ErrorCode::IoError,
               "SourceReadFailed should map to IoError") &&
           expect_true(
               to_error_code(RuntimeErrorCode::UnsupportedSourceType) == ErrorCode::NotSupported,
               "UnsupportedSourceType should map to NotSupported") &&
           expect_true(
               to_error_code(RuntimeErrorCode::ManifestParseFailed) == ErrorCode::ParseError,
               "ManifestParseFailed should map to ParseError") &&
           expect_true(
               to_error_code(RuntimeErrorCode::PermissionDenied) == ErrorCode::PermissionDenied,
               "PermissionDenied should map to PermissionDenied") &&
           expect_true(
               to_error_code(RuntimeErrorCode::ProcessFailed) == ErrorCode::ExternalError,
               "ProcessFailed should map to ExternalError") &&
           expect_true(
               to_error_code(RuntimeErrorCode::Timeout) == ErrorCode::Timeout,
               "Timeout should map to Timeout") &&
           expect_true(
               to_error_code(RuntimeErrorCode::Cancelled) == ErrorCode::Cancelled,
               "Cancelled should map to Cancelled") &&
           expect_true(
               to_error_code(RuntimeErrorCode::NotImplemented) == ErrorCode::NotImplemented,
               "NotImplemented should map to NotImplemented") &&
           expect_true(
               to_error_code(RuntimeErrorCode::InternalError) == ErrorCode::InternalError,
               "InternalError should map to InternalError");
  }

  bool test_runtime_error_string_names()
  {
    using kordex::runtime::RuntimeErrorCode;
    using kordex::runtime::to_string;

    return expect_true(
               std::string_view(to_string(RuntimeErrorCode::None)) == "none",
               "None string should be none") &&
           expect_true(
               std::string_view(to_string(RuntimeErrorCode::InvalidArgument)) == "invalid_argument",
               "InvalidArgument string should be invalid_argument") &&
           expect_true(
               std::string_view(to_string(RuntimeErrorCode::SourceNotFound)) == "source_not_found",
               "SourceNotFound string should be source_not_found") &&
           expect_true(
               std::string_view(to_string(RuntimeErrorCode::ManifestParseFailed)) == "manifest_parse_failed",
               "ManifestParseFailed string should be manifest_parse_failed") &&
           expect_true(
               std::string_view(to_string(RuntimeErrorCode::ProcessFailed)) == "process_failed",
               "ProcessFailed string should be process_failed") &&
           expect_true(
               std::string_view(to_string(RuntimeErrorCode::InternalError)) == "internal_error",
               "InternalError string should be internal_error");
  }

  bool test_make_runtime_error()
  {
    const auto error = kordex::runtime::make_runtime_error(
        kordex::runtime::RuntimeErrorCode::SourceNotFound,
        "source file was not found");

    return expect_true(
               error.has_error(),
               "runtime error should report an error") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::NotFound,
               "runtime error should use mapped error code") &&
           expect_true(
               error.category() == kordex::runtime::runtime_error_category(),
               "runtime error should use runtime category") &&
           expect_true(
               std::string_view(error.message()) == "source file was not found",
               "runtime error should keep message");
  }

  bool test_ok_error()
  {
    const auto error = kordex::runtime::ok();

    return expect_true(
        !error.has_error(),
        "ok error should not report an error");
  }

  bool run_tests()
  {
    return test_runtime_error_category() &&
           test_runtime_error_code_mapping() &&
           test_runtime_error_string_names() &&
           test_make_runtime_error() &&
           test_ok_error();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
