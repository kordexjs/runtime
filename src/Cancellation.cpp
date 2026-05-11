/**
 *
 *  @file Cancellation.cpp
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

#include <kordex/runtime/Cancellation.hpp>

namespace kordex::runtime
{
  Cancellation::Cancellation()
      : source_()
  {
  }

  CancelToken Cancellation::token() const noexcept
  {
    return source_.token();
  }

  void Cancellation::request_cancel() noexcept
  {
    source_.request_cancel();
  }

  bool Cancellation::is_cancelled() const noexcept
  {
    return source_.is_cancelled();
  }

  bool Cancellation::can_cancel() const noexcept
  {
    return token().can_cancel();
  }

  Error Cancellation::error(
      const char *message) const
  {
    return make_cancelled_error(message);
  }

  Error make_cancelled_error(
      const char *message)
  {
    return make_runtime_error(
        RuntimeErrorCode::Cancelled,
        message ? message : "runtime operation was cancelled");
  }

} // namespace kordex::runtime
