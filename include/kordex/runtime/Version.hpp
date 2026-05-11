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
  inline constexpr int version_major_value = 0;
  inline constexpr int version_minor_value = 1;
  inline constexpr int version_patch_value = 0;

  /**
   * @brief Kordex runtime version as a stable string.
   */
  inline constexpr std::string_view version_string = "0.1.0";

  /**
   * @brief Return the Kordex runtime major version.
   */
  [[nodiscard]] constexpr int version_major() noexcept
  {
    return version_major_value;
  }

  /**
   * @brief Return the Kordex runtime minor version.
   */
  [[nodiscard]] constexpr int version_minor() noexcept
  {
    return version_minor_value;
  }

  /**
   * @brief Return the Kordex runtime patch version.
   */
  [[nodiscard]] constexpr int version_patch() noexcept
  {
    return version_patch_value;
  }

  /**
   * @brief Return the Kordex runtime version string.
   */
  [[nodiscard]] constexpr std::string_view version() noexcept
  {
    return version_string;
  }

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_VERSION_HPP
