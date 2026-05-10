/**
 *
 *  @file test_cancellation.cpp
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

#include <kordex/runtime/Cancellation.hpp>

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

  bool test_cancellation_initial_state()
  {
    kordex::runtime::Cancellation cancellation;
    const auto token = cancellation.token();

    return expect_true(
               cancellation.can_cancel(),
               "cancellation controller should be cancellable") &&
           expect_true(
               token.can_cancel(),
               "cancellation token should be cancellable") &&
           expect_true(
               !cancellation.is_cancelled(),
               "cancellation should not be requested initially") &&
           expect_true(
               !token.is_cancelled(),
               "token should not be cancelled initially");
  }

  bool test_request_cancel_updates_controller_and_token()
  {
    kordex::runtime::Cancellation cancellation;
    const auto token = cancellation.token();

    cancellation.request_cancel();

    return expect_true(
               cancellation.is_cancelled(),
               "cancellation controller should report cancelled") &&
           expect_true(
               token.is_cancelled(),
               "token should observe cancellation");
  }

  bool test_request_cancel_is_idempotent()
  {
    kordex::runtime::Cancellation cancellation;

    cancellation.request_cancel();
    cancellation.request_cancel();
    cancellation.request_cancel();

    return expect_true(
        cancellation.is_cancelled(),
        "request_cancel should remain cancelled after repeated calls");
  }

  bool test_make_cancelled_error()
  {
    const auto error = kordex::runtime::make_cancelled_error(
        "operation cancelled by test");

    return expect_true(
               error.has_error(),
               "cancelled error should report an error") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::Cancelled,
               "cancelled error should use Cancelled error code") &&
           expect_true(
               error.category() == kordex::runtime::runtime_error_category(),
               "cancelled error should use runtime category") &&
           expect_true(
               std::string_view(error.message()) == "operation cancelled by test",
               "cancelled error should preserve custom message");
  }

  bool test_make_cancelled_error_fallback_message()
  {
    const auto error = kordex::runtime::make_cancelled_error(nullptr);

    return expect_true(
               error.has_error(),
               "cancelled error should report an error") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::Cancelled,
               "cancelled error should use Cancelled error code") &&
           expect_true(
               std::string_view(error.message()) == "runtime operation was cancelled",
               "cancelled error should use fallback message");
  }

  bool test_cancellation_error_method()
  {
    kordex::runtime::Cancellation cancellation;

    const auto error = cancellation.error("runtime shutdown requested");

    return expect_true(
               error.has_error(),
               "cancellation error should report an error") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::Cancelled,
               "cancellation error should use Cancelled error code") &&
           expect_true(
               std::string_view(error.message()) == "runtime shutdown requested",
               "cancellation error should preserve message");
  }

  bool run_tests()
  {
    return test_cancellation_initial_state() &&
           test_request_cancel_updates_controller_and_token() &&
           test_request_cancel_is_idempotent() &&
           test_make_cancelled_error() &&
           test_make_cancelled_error_fallback_message() &&
           test_cancellation_error_method();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
