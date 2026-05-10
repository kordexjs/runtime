/**
 *
 *  @file Diagnostics.hpp
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

#ifndef KORDEX_RUNTIME_DIAGNOSTICS_HPP
#define KORDEX_RUNTIME_DIAGNOSTICS_HPP

#include <string>
#include <string_view>
#include <vector>

#include <vix/json/json.hpp>
#include <vix/log/Log.hpp>

#include <kordex/runtime/Clock.hpp>
#include <kordex/runtime/Error.hpp>
#include <kordex/runtime/RuntimeResult.hpp>

namespace kordex::runtime
{
  /**
   * @enum DiagnosticLevel
   * @brief Runtime diagnostic severity level.
   */
  enum class DiagnosticLevel
  {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical
  };

  /**
   * @struct DiagnosticEvent
   * @brief Structured runtime diagnostic event.
   */
  struct DiagnosticEvent
  {
    /**
     * @brief Event severity level.
     */
    DiagnosticLevel level{DiagnosticLevel::Info};

    /**
     * @brief Stable event name.
     *
     * Examples:
     * - runtime_start
     * - source_load
     * - module_resolve
     * - process_spawn
     * - runtime_error
     */
    std::string name{};

    /**
     * @brief Human-readable event message.
     */
    std::string message{};

    /**
     * @brief Optional runtime/module path related to this event.
     */
    std::string path{};

    /**
     * @brief Event timestamp.
     */
    Timestamp timestamp{};

    /**
     * @brief Optional structured details.
     */
    vix::json::Json details{vix::json::Json::object()};

    /**
     * @brief Create an event.
     */
    [[nodiscard]] static DiagnosticEvent create(
        DiagnosticLevel level,
        std::string name,
        std::string message);

    /**
     * @brief Return true if this event contains structured details.
     */
    [[nodiscard]] bool has_details() const noexcept;

    /**
     * @brief Convert the event to JSON.
     */
    [[nodiscard]] vix::json::Json to_json() const;
  };

  /**
   * @class Diagnostics
   * @brief Runtime diagnostics collector and logging facade.
   *
   * Diagnostics keeps structured events in memory and also forwards events
   * to the Vix logging facade.
   */
  class Diagnostics
  {
  public:
    /**
     * @brief Construct diagnostics with enabled state.
     */
    explicit Diagnostics(bool enabled = true);

    /**
     * @brief Enable diagnostics.
     */
    void enable() noexcept;

    /**
     * @brief Disable diagnostics.
     */
    void disable() noexcept;

    /**
     * @brief Return true if diagnostics are enabled.
     */
    [[nodiscard]] bool enabled() const noexcept;

    /**
     * @brief Record a diagnostic event.
     */
    void record(DiagnosticEvent event);

    /**
     * @brief Record a simple event.
     */
    void record(
        DiagnosticLevel level,
        std::string name,
        std::string message);

    /**
     * @brief Record a runtime error.
     */
    void record_error(
        std::string name,
        const Error &error);

    /**
     * @brief Record a runtime result.
     */
    void record_result(
        std::string name,
        const RuntimeResult &result);

    /**
     * @brief Clear all recorded events.
     */
    void clear();

    /**
     * @brief Return the number of recorded events.
     */
    [[nodiscard]] std::size_t size() const noexcept;

    /**
     * @brief Return true if no event is recorded.
     */
    [[nodiscard]] bool empty() const noexcept;

    /**
     * @brief Access recorded events.
     */
    [[nodiscard]] const std::vector<DiagnosticEvent> &events() const noexcept;

    /**
     * @brief Convert all events to JSON.
     */
    [[nodiscard]] vix::json::Json to_json() const;

  private:
    bool enabled_{true};
    std::vector<DiagnosticEvent> events_{};
  };

  /**
   * @brief Convert DiagnosticLevel to a stable string.
   */
  [[nodiscard]] const char *to_string(DiagnosticLevel level) noexcept;

  /**
   * @brief Convert DiagnosticLevel to Vix log level.
   */
  [[nodiscard]] vix::log::LogLevel to_log_level(DiagnosticLevel level) noexcept;

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_DIAGNOSTICS_HPP
