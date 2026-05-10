/**
 *
 *  @file Result.hpp
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

#ifndef KORDEX_RUNTIME_RESULT_HPP
#define KORDEX_RUNTIME_RESULT_HPP

#include <vix/error/Result.hpp>

#include <kordex/runtime/Error.hpp>

namespace kordex::runtime
{
  /**
   * @brief Kordex runtime result type.
   *
   * This is a thin alias over the shared Vix Result<T> type.
   * It represents either:
   * - a valid value of type T
   * - a structured Error
   *
   * Runtime code should prefer Result<T> for recoverable failures instead of
   * throwing exceptions.
   */
  template <typename T>
  using Result = vix::error::Result<T>;

  /**
   * @brief Result type for operations that only need to report success/failure.
   */
  using BoolResult = Result<bool>;

  /**
   * @brief Result type for string-producing runtime operations.
   */
  using StringResult = Result<std::string>;

  /**
   * @brief Result type for integer-producing runtime operations.
   */
  using IntResult = Result<int>;

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_RESULT_HPP
