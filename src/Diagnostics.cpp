/**
 *
 *  @file Diagnostics.cpp
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

#include <utility>
#include <kordex/runtime/Diagnostics.hpp>

namespace kordex::runtime
{
  DiagnosticEvent DiagnosticEvent::create(
      DiagnosticLevel level_value,
      std::string name_value,
      std::string message_value)
  {
    DiagnosticEvent event;
    event.level = level_value;
    event.name = std::move(name_value);
    event.message = std::move(message_value);
    event.timestamp = Clock::now();
    event.details = vix::json::Json::object();
    return event;
  }

  bool DiagnosticEvent::has_details() const noexcept
  {
    return details.is_object() && !details.empty();
  }

  vix::json::Json DiagnosticEvent::to_json() const
  {
    vix::json::Json json = vix::json::Json::object();

    json["level"] = to_string(level);
    json["name"] = name;
    json["message"] = message;
    json["path"] = path;
    json["timestamp_ns"] = timestamp.nanoseconds_since_epoch();

    if (has_details())
    {
      json["details"] = details;
    }
    else
    {
      json["details"] = vix::json::Json::object();
    }

    return json;
  }

  Diagnostics::Diagnostics(bool enabled)
      : enabled_(enabled)
  {
  }

  void Diagnostics::enable() noexcept
  {
    enabled_ = true;
  }

  void Diagnostics::disable() noexcept
  {
    enabled_ = false;
  }

  bool Diagnostics::enabled() const noexcept
  {
    return enabled_;
  }

  void Diagnostics::record(DiagnosticEvent event)
  {
    if (!enabled_)
    {
      return;
    }

    if (event.timestamp.is_zero())
    {
      event.timestamp = Clock::now();
    }

    vix::log::logf(
        to_log_level(event.level),
        event.message,
        "event",
        event.name,
        "module",
        "kordex.runtime");

    events_.push_back(std::move(event));
  }

  void Diagnostics::record(
      DiagnosticLevel level,
      std::string name,
      std::string message)
  {
    record(DiagnosticEvent::create(
        level,
        std::move(name),
        std::move(message)));
  }

  void Diagnostics::record_error(
      std::string name,
      const Error &error)
  {
    DiagnosticEvent event = DiagnosticEvent::create(
        DiagnosticLevel::Error,
        std::move(name),
        std::string(error.message()));

    event.details["code"] = static_cast<int>(error.code());
    event.details["category"] = std::string(error.category().name());
    event.details["message"] = std::string(error.message());

    record(std::move(event));
  }

  void Diagnostics::record_result(
      std::string name,
      const RuntimeResult &result)
  {
    DiagnosticLevel level = DiagnosticLevel::Info;

    if (result.failed() || result.error.has_error())
    {
      level = DiagnosticLevel::Error;
    }
    else if (result.was_cancelled() || result.timed_out())
    {
      level = DiagnosticLevel::Warning;
    }

    DiagnosticEvent event = DiagnosticEvent::create(
        level,
        std::move(name),
        std::string("runtime result: ") + to_string(result.status));

    event.details["status"] = to_string(result.status);
    event.details["exit_code"] = result.exit_code;
    event.details["duration_ns"] = result.duration.count_ns();
    event.details["has_error"] = result.error.has_error();

    if (result.error.has_error())
    {
      event.details["error"] = {
          {"code", static_cast<int>(result.error.code())},
          {"category", std::string(result.error.category().name())},
          {"message", std::string(result.error.message())},
      };
    }

    record(std::move(event));
  }

  void Diagnostics::clear()
  {
    events_.clear();
  }

  std::size_t Diagnostics::size() const noexcept
  {
    return events_.size();
  }

  bool Diagnostics::empty() const noexcept
  {
    return events_.empty();
  }

  const std::vector<DiagnosticEvent> &Diagnostics::events() const noexcept
  {
    return events_;
  }

  vix::json::Json Diagnostics::to_json() const
  {
    vix::json::Json array = vix::json::Json::array();

    for (const auto &event : events_)
    {
      array.push_back(event.to_json());
    }

    return array;
  }

  const char *to_string(DiagnosticLevel level) noexcept
  {
    switch (level)
    {
    case DiagnosticLevel::Trace:
      return "trace";
    case DiagnosticLevel::Debug:
      return "debug";
    case DiagnosticLevel::Info:
      return "info";
    case DiagnosticLevel::Warning:
      return "warning";
    case DiagnosticLevel::Error:
      return "error";
    case DiagnosticLevel::Critical:
      return "critical";
    }

    return "info";
  }

  vix::log::LogLevel to_log_level(DiagnosticLevel level) noexcept
  {
    switch (level)
    {
    case DiagnosticLevel::Trace:
      return vix::log::LogLevel::Trace;
    case DiagnosticLevel::Debug:
      return vix::log::LogLevel::Debug;
    case DiagnosticLevel::Info:
      return vix::log::LogLevel::Info;
    case DiagnosticLevel::Warning:
      return vix::log::LogLevel::Warn;
    case DiagnosticLevel::Error:
      return vix::log::LogLevel::Error;
    case DiagnosticLevel::Critical:
      return vix::log::LogLevel::Critical;
    }

    return vix::log::LogLevel::Info;
  }

} // namespace kordex::runtime
