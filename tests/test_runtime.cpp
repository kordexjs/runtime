/**
 *
 *  @file test_runtime.cpp
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

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

#include <kordex/runtime/Runtime.hpp>

namespace
{
  bool expect_true(bool condition, const char *message)
  {
    if (!condition)
    {
      std::cerr << "[test_runtime] failed: "
                << (message ? message : "unknown assertion")
                << '\n';

      return false;
    }

    return true;
  }

  std::filesystem::path test_dir()
  {
    return std::filesystem::temp_directory_path() /
           "kordex_runtime_runtime_tests";
  }

  std::filesystem::path write_test_file(
      const std::string &relative_path,
      const std::string &content)
  {
    const auto path = test_dir() / relative_path;

    std::filesystem::create_directories(path.parent_path());

    std::ofstream out(path, std::ios::trunc);
    out << content;

    return path;
  }

  void cleanup_test_files()
  {
    std::error_code ec;
    std::filesystem::remove_all(test_dir(), ec);
  }

  kordex::runtime::RuntimeConfig make_test_config()
  {
    kordex::runtime::RuntimeOptions options =
        kordex::runtime::RuntimeOptions::test();

    options.workers = 1;
    options.diagnostics = true;
    options.allow_process = true;

    auto config = kordex::runtime::RuntimeConfig::from_options(options);
    if (!config)
    {
      return {};
    }

    return config.value();
  }

  bool wait_until_true(
      const std::atomic<bool> &flag,
      int max_attempts = 50)
  {
    for (int attempt = 0; attempt < max_attempts; ++attempt)
    {
      if (flag.load(std::memory_order_acquire))
      {
        return true;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return false;
  }

  bool test_default_runtime()
  {
    kordex::runtime::Runtime runtime;

    return expect_true(
               !runtime.is_running(),
               "default runtime should not be running") &&
           expect_true(
               runtime.state() == kordex::runtime::RuntimeLifecycleState::Created,
               "default runtime state should be created") &&
           expect_true(
               !runtime.config().allow_process,
               "default runtime should keep process disabled") &&
           expect_true(
               runtime.module_resolver().options().base_dir == ".",
               "default module resolver base dir should be current directory");
  }

  bool test_runtime_from_config()
  {
    const auto config = make_test_config();

    kordex::runtime::Runtime runtime(config);

    return expect_true(
               !runtime.is_running(),
               "configured runtime should not start automatically") &&
           expect_true(
               runtime.config().workers == 1,
               "configured runtime should preserve workers") &&
           expect_true(
               runtime.config().diagnostics,
               "configured runtime should preserve diagnostics") &&
           expect_true(
               runtime.config().allow_process,
               "configured runtime should preserve process permission");
  }

  bool test_runtime_from_options()
  {
    kordex::runtime::RuntimeOptions options =
        kordex::runtime::RuntimeOptions::test();

    options.workers = 1;
    options.diagnostics = true;
    options.allow_process = true;

    auto result = kordex::runtime::Runtime::from_options(options);

    if (!result)
    {
      return false;
    }

    return expect_true(
               result.value().config().workers == 1,
               "from_options should preserve workers") &&
           expect_true(
               result.value().config().diagnostics,
               "from_options should preserve diagnostics") &&
           expect_true(
               result.value().config().allow_process,
               "from_options should preserve allow_process") &&
           expect_true(
               !result.value().is_running(),
               "from_options should not start runtime automatically");
  }

  bool test_development_runtime_factory()
  {
    auto runtime = kordex::runtime::Runtime::development();

    return expect_true(
               !runtime.is_running(),
               "development runtime should not start automatically") &&
           expect_true(
               runtime.config().mode == kordex::runtime::RuntimeMode::Development,
               "development runtime should use development mode");
  }

  bool test_test_runtime_factory()
  {
    auto runtime = kordex::runtime::Runtime::test();

    return expect_true(
               !runtime.is_running(),
               "test runtime should not start automatically") &&
           expect_true(
               runtime.config().mode == kordex::runtime::RuntimeMode::Test,
               "test runtime should use test mode");
  }

  bool test_start_runtime()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    const auto error = runtime.start();

    const bool ok =
        !error.has_error() &&
        runtime.is_running() &&
        runtime.state() == kordex::runtime::RuntimeLifecycleState::Running;

    (void)runtime.shutdown();

    return expect_true(
        ok,
        "runtime should start successfully");
  }

  bool test_start_rejects_when_already_running()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    const auto first = runtime.start();
    const auto second = runtime.start();

    const bool ok =
        !first.has_error() &&
        second.has_error() &&
        second.code() == kordex::runtime::ErrorCode::InvalidState &&
        runtime.is_running();

    (void)runtime.shutdown();

    return expect_true(
        ok,
        "runtime should reject second start while running");
  }

  bool test_stop_runtime()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    const auto start_error = runtime.start();
    const auto stop_error = runtime.stop();

    return expect_true(
               !start_error.has_error(),
               "runtime should start before stop") &&
           expect_true(
               !stop_error.has_error(),
               "runtime should stop successfully") &&
           expect_true(
               !runtime.is_running(),
               "runtime should not be running after stop") &&
           expect_true(
               runtime.state() == kordex::runtime::RuntimeLifecycleState::Stopped,
               "runtime state should be stopped after stop");
  }

  bool test_shutdown_runtime()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    const auto start_error = runtime.start();
    const auto shutdown_error = runtime.shutdown();

    return expect_true(
               !start_error.has_error(),
               "runtime should start before shutdown") &&
           expect_true(
               !shutdown_error.has_error(),
               "runtime should shutdown successfully") &&
           expect_true(
               !runtime.is_running(),
               "runtime should not be running after shutdown") &&
           expect_true(
               runtime.state() == kordex::runtime::RuntimeLifecycleState::Stopped,
               "runtime state should be stopped after shutdown");
  }

  bool test_load_source()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "src/app.js",
        "console.log('hello runtime');");

    kordex::runtime::Runtime runtime(make_test_config());

    auto result = runtime.load_source(path.string());

    cleanup_test_files();

    if (!result)
    {
      return false;
    }

    const auto &source = result.value();

    return expect_true(
               source.path == path.string(),
               "load_source should load expected path") &&
           expect_true(
               source.is_javascript(),
               "load_source should detect JavaScript") &&
           expect_true(
               source.executable(),
               "loaded JavaScript source should be executable") &&
           expect_true(
               source.content == "console.log('hello runtime');",
               "load_source should preserve content");
  }

  bool test_load_source_rejects_missing_file()
  {
    cleanup_test_files();

    kordex::runtime::Runtime runtime(make_test_config());

    const auto result = runtime.load_source(
        (test_dir() / "missing.js").string());

    return expect_true(
               !result,
               "load_source should reject missing file") &&
           expect_true(
               result.error().code() == kordex::runtime::ErrorCode::NotFound,
               "missing source should use NotFound");
  }

  bool test_resolve_builtin_module()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    auto result = runtime.resolve_module("kordex:fs");

    if (!result)
    {
      return false;
    }

    return expect_true(
               result.value().is_builtin(),
               "resolve_module should resolve builtin module") &&
           expect_true(
               result.value().builtin_name == "fs",
               "resolve_module should preserve builtin name");
  }

  bool test_resolve_package_module()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    auto result = runtime.resolve_module("react");

    if (!result)
    {
      return false;
    }

    return expect_true(
               result.value().is_package(),
               "resolve_module should resolve package module") &&
           expect_true(
               result.value().package_name == "react",
               "resolve_module should preserve package name");
  }

  bool test_run_source_success()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    auto source = kordex::runtime::SourceFile::from_content(
        "app.js",
        "console.log('hello');");

    if (!source)
    {
      return false;
    }

    const auto result = runtime.run_source(source.value());

    return expect_true(
               result.succeeded(),
               "run_source should succeed for executable source") &&
           expect_true(
               result.status == kordex::runtime::RuntimeExitStatus::Success,
               "run_source should use success status") &&
           expect_true(
               result.exit_code == 0,
               "run_source should use zero exit code") &&
           expect_true(
               !result.started_at.is_zero(),
               "run_source should set started_at") &&
           expect_true(
               !result.finished_at.is_zero(),
               "run_source should set finished_at");
  }

  bool test_run_source_rejects_json()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    auto source = kordex::runtime::SourceFile::from_content(
        "package.json",
        R"({"name":"demo"})");

    if (!source)
    {
      return false;
    }

    const auto result = runtime.run_source(source.value());

    return expect_true(
               !result.succeeded(),
               "run_source should reject non-executable source") &&
           expect_true(
               result.failed(),
               "run_source JSON should fail") &&
           expect_true(
               result.error.code() == kordex::runtime::ErrorCode::NotSupported,
               "run_source JSON should use NotSupported");
  }

  bool test_run_file_success()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "run/app.js",
        "console.log('run file');");

    kordex::runtime::Runtime runtime(make_test_config());

    const auto result = runtime.run_file(path.string());

    cleanup_test_files();

    return expect_true(
               result.succeeded(),
               "run_file should succeed for JavaScript file") &&
           expect_true(
               result.status == kordex::runtime::RuntimeExitStatus::Success,
               "run_file should use success status") &&
           expect_true(
               result.exit_code == 0,
               "run_file should use zero exit code");
  }

  bool test_run_file_rejects_missing_file()
  {
    cleanup_test_files();

    kordex::runtime::Runtime runtime(make_test_config());

    const auto result = runtime.run_file(
        (test_dir() / "missing.js").string());

    return expect_true(
               !result.succeeded(),
               "run_file should reject missing file") &&
           expect_true(
               result.failed(),
               "run_file missing file should fail") &&
           expect_true(
               result.error.code() == kordex::runtime::ErrorCode::NotFound,
               "run_file missing file should use NotFound");
  }

  bool test_post_executes_task()
  {
    kordex::runtime::Runtime runtime(make_test_config());
    std::atomic<bool> executed{false};

    const auto start_error = runtime.start();

    const auto post_error = runtime.post(
        [&executed]()
        {
          executed.store(true, std::memory_order_release);
        });

    const bool did_execute = wait_until_true(executed);

    const auto shutdown_error = runtime.shutdown();

    return expect_true(
               !start_error.has_error(),
               "runtime should start before post") &&
           expect_true(
               !post_error.has_error(),
               "runtime should accept posted task") &&
           expect_true(
               did_execute,
               "runtime should execute posted task") &&
           expect_true(
               !shutdown_error.has_error(),
               "runtime should shutdown after post");
  }

  bool test_post_rejects_when_not_running()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    const auto error = runtime.post([]() {});

    return expect_true(
               error.has_error(),
               "runtime should reject post when not running") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::InvalidState,
               "post when not running should use InvalidState");
  }

  bool test_run_task_success()
  {
    std::atomic<bool> executed{false};

    kordex::runtime::Task task(
        [&executed]()
        {
          executed.store(true, std::memory_order_release);
        });

    kordex::runtime::Runtime runtime(make_test_config());

    const auto result = runtime.run_task(std::move(task));

    return expect_true(
               executed.load(std::memory_order_acquire),
               "run_task should execute task") &&
           expect_true(
               result.succeeded(),
               "run_task should return success") &&
           expect_true(
               result.exit_code == 0,
               "run_task should use zero exit code");
  }

  bool test_run_task_failure()
  {
    kordex::runtime::Task task(
        []()
        {
          throw std::runtime_error("runtime task failed");
        });

    kordex::runtime::Runtime runtime(make_test_config());

    const auto result = runtime.run_task(std::move(task));

    return expect_true(
               !result.succeeded(),
               "run_task should return failure for throwing task") &&
           expect_true(
               result.failed(),
               "throwing task should fail") &&
           expect_true(
               result.error.code() == kordex::runtime::ErrorCode::InternalError,
               "throwing task should use InternalError");
  }

  bool test_load_manifest()
  {
    cleanup_test_files();

    const auto path = write_test_file(
        "kordex.json",
        R"({
  "name": "demo",
  "version": "0.1.0",
  "entry": "src/main.js"
})");

    kordex::runtime::Runtime runtime(make_test_config());

    auto result = runtime.load_manifest(path.string());

    cleanup_test_files();

    if (!result)
    {
      return false;
    }

    return expect_true(
               result.value().name == "demo",
               "load_manifest should preserve manifest name") &&
           expect_true(
               result.value().version == "0.1.0",
               "load_manifest should preserve manifest version") &&
           expect_true(
               result.value().entry == "src/main.js",
               "load_manifest should preserve manifest entry");
  }

  bool test_process_facade()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    auto process = runtime.process();

    return expect_true(
               process.allowed(),
               "runtime process facade should use runtime config") &&
           expect_true(
               process.config().allow_process,
               "runtime process facade should preserve allow_process");
  }

  bool test_diagnostics_access()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    runtime.diagnostics().record(
        kordex::runtime::DiagnosticLevel::Info,
        "runtime_manual_event",
        "manual runtime diagnostic");

    return expect_true(
               runtime.diagnostics().size() == 1,
               "runtime diagnostics should record event") &&
           expect_true(
               runtime.diagnostics().events().front().name == "runtime_manual_event",
               "runtime diagnostics should preserve event name");
  }

  bool test_loop_access()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    auto &loop = runtime.loop();

    return expect_true(
               !loop.is_running(),
               "runtime loop accessor should expose loop") &&
           expect_true(
               &loop == &runtime.loop(),
               "runtime loop accessor should return stable reference");
  }

  bool test_move_constructor()
  {
    kordex::runtime::Runtime runtime(make_test_config());

    auto moved = std::move(runtime);

    return expect_true(
               !moved.is_running(),
               "moved runtime should be usable") &&
           expect_true(
               moved.config().workers == 1,
               "moved runtime should preserve config");
  }

  bool test_move_assignment()
  {
    kordex::runtime::Runtime left(make_test_config());
    kordex::runtime::Runtime right;

    right = std::move(left);

    return expect_true(
               !right.is_running(),
               "move-assigned runtime should be usable") &&
           expect_true(
               right.config().workers == 1,
               "move-assigned runtime should preserve config");
  }

  bool run_test(
      const char *name,
      bool (*test)())
  {
    if (!test())
    {
      std::cerr << "[test_runtime] failed test: "
                << (name ? name : "unknown")
                << '\n';

      return false;
    }

    return true;
  }

  bool run_tests()
  {
    cleanup_test_files();

    const bool ok =
        run_test("test_default_runtime", test_default_runtime) &&
        run_test("test_runtime_from_config", test_runtime_from_config) &&
        run_test("test_runtime_from_options", test_runtime_from_options) &&
        run_test("test_development_runtime_factory", test_development_runtime_factory) &&
        run_test("test_test_runtime_factory", test_test_runtime_factory) &&
        run_test("test_start_runtime", test_start_runtime) &&
        run_test("test_start_rejects_when_already_running", test_start_rejects_when_already_running) &&
        run_test("test_stop_runtime", test_stop_runtime) &&
        run_test("test_shutdown_runtime", test_shutdown_runtime) &&
        run_test("test_load_source", test_load_source) &&
        run_test("test_load_source_rejects_missing_file", test_load_source_rejects_missing_file) &&
        run_test("test_resolve_builtin_module", test_resolve_builtin_module) &&
        run_test("test_resolve_package_module", test_resolve_package_module) &&
        run_test("test_run_source_success", test_run_source_success) &&
        run_test("test_run_source_rejects_json", test_run_source_rejects_json) &&
        run_test("test_run_file_success", test_run_file_success) &&
        run_test("test_run_file_rejects_missing_file", test_run_file_rejects_missing_file) &&
        run_test("test_post_executes_task", test_post_executes_task) &&
        run_test("test_post_rejects_when_not_running", test_post_rejects_when_not_running) &&
        run_test("test_run_task_success", test_run_task_success) &&
        run_test("test_run_task_failure", test_run_task_failure) &&
        run_test("test_load_manifest", test_load_manifest) &&
        run_test("test_process_facade", test_process_facade) &&
        run_test("test_diagnostics_access", test_diagnostics_access) &&
        run_test("test_loop_access", test_loop_access) &&
        run_test("test_move_constructor", test_move_constructor) &&
        run_test("test_move_assignment", test_move_assignment);

    cleanup_test_files();

    return ok;
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
