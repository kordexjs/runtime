/**
 *
 *  @file Version.hpp
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

#ifndef KORDEX_RUNTIME_VERSION_HPP
#define KORDEX_RUNTIME_VERSION_HPP

#include <string_view>

namespace kordex::runtime
{
  /**
   * @brief Kordex runtime semantic version numbers.
   */
  inline constexpr int KORDEX_RUNTIME_VERSION_MAJOR = 0;
  inline constexpr int KORDEX_RUNTIME_VERSION_MINOR = 1;
  inline constexpr int KORDEX_RUNTIME_VERSION_PATCH = 0;

  /**
   * @brief Kordex runtime version as a stable string.
   */
  inline constexpr std::string_view KORDEX_RUNTIME_VERSION = "0.1.0";

  /**
   * @brief Return the Kordex runtime major version.
   */
  [[nodiscard]] constexpr int version_major() noexcept
  {
    return KORDEX_RUNTIME_VERSION_MAJOR;
  }

  /**
   * @brief Return the Kordex runtime minor version.
   */
  [[nodiscard]] constexpr int version_minor() noexcept
  {
    return KORDEX_RUNTIME_VERSION_MINOR;
  }

  /**
   * @brief Return the Kordex runtime patch version.
   */
  [[nodiscard]] constexpr int version_patch() noexcept
  {
    return KORDEX_RUNTIME_VERSION_PATCH;
  }

  /**
   * @brief Return the Kordex runtime version string.
   */
  [[nodiscard]] constexpr std::string_view version() noexcept
  {
    return KORDEX_RUNTIME_VERSION;
  }

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_VERSION_HPP
