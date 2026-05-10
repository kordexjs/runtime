/**
 *
 *  @file Runtime.cpp
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

#include <utility>
#include <kordex/runtime/Runtime.hpp>

namespace kordex::runtime
{
  namespace
  {
    [[nodiscard]] ModuleResolver make_module_resolver(
        const RuntimeConfig &config)
    {
      (void)config;

      ModuleResolverOptions options;
      options.base_dir = ".";
      options.allow_builtins = true;
      options.allow_packages = true;
      options.resolve_extensions = true;
      options.resolve_index = true;

      return ModuleResolver(options);
    }
  } // namespace

  Runtime::Runtime()
      : Runtime(RuntimeConfig{}, false)
  {
  }

  Runtime::Runtime(RuntimeConfig config)
      : Runtime(std::move(config), false)
  {
  }

  Runtime::Runtime(RuntimeConfig config, bool start_loop)
      : config_(std::move(config)),
        module_resolver_(make_module_resolver(config_)),
        loop_(std::make_unique<RuntimeLoop>(config_))
  {
    if (start_loop)
    {
      (void)start();
    }
  }

  Runtime::Runtime(Runtime &&other) noexcept
      : config_(std::move(other.config_)),
        module_resolver_(std::move(other.module_resolver_)),
        loop_(std::move(other.loop_))
  {
  }

  Runtime &Runtime::operator=(Runtime &&other) noexcept
  {
    if (this != &other)
    {
      if (loop_)
      {
        (void)loop_->shutdown();
      }

      config_ = std::move(other.config_);
      module_resolver_ = std::move(other.module_resolver_);
      loop_ = std::move(other.loop_);
    }

    return *this;
  }

  Runtime::~Runtime()
  {
    if (loop_)
    {
      (void)loop_->shutdown();
    }
  }

  Result<Runtime> Runtime::from_options(
      const RuntimeOptions &options)
  {
    auto config = RuntimeConfig::from_options(options);
    if (!config)
    {
      return config.error();
    }

    return Runtime(config.value());
  }

  Runtime Runtime::development()
  {
    auto config = RuntimeConfig::from_options(
        RuntimeOptions::development());

    if (!config)
    {
      return Runtime{};
    }

    return Runtime(config.value());
  }

  Runtime Runtime::test()
  {
    auto config = RuntimeConfig::from_options(
        RuntimeOptions::test());

    if (!config)
    {
      return Runtime{};
    }

    return Runtime(config.value());
  }

  Error Runtime::start()
  {
    if (!loop_)
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidState,
          "runtime loop is not initialized");
    }

    return loop_->start();
  }

  Error Runtime::stop()
  {
    if (!loop_)
    {
      return ok();
    }

    return loop_->stop();
  }

  Error Runtime::shutdown()
  {
    if (!loop_)
    {
      return ok();
    }

    return loop_->shutdown();
  }

  bool Runtime::is_running() const noexcept
  {
    return loop_ && loop_->is_running();
  }

  RuntimeLifecycleState Runtime::state() const noexcept
  {
    if (!loop_)
    {
      return RuntimeLifecycleState::Stopped;
    }

    return loop_->state();
  }

  Result<SourceFile> Runtime::load_source(
      const std::string &path) const
  {
    return SourceFile::load(path);
  }

  Result<ResolvedModule> Runtime::resolve_module(
      const std::string &specifier) const
  {
    return module_resolver_.resolve(specifier);
  }

  RuntimeResult Runtime::run_file(
      const std::string &path)
  {
    RuntimeResult result;
    result.mark_started();

    auto source = load_source(path);
    if (!source)
    {
      result = RuntimeResult::failure(source.error(), 1);
      result.mark_finished();

      if (loop_)
      {
        loop_->diagnostics().record_result(
            "runtime_run_file_failed",
            result);
      }

      return result;
    }

    return run_source(source.value());
  }

  RuntimeResult Runtime::run_source(
      SourceFile source)
  {
    RuntimeResult result;
    result.mark_started();

    const auto validation = source.validate();
    if (validation)
    {
      result = RuntimeResult::failure(validation, 1);
      result.mark_finished();

      if (loop_)
      {
        loop_->diagnostics().record_result(
            "runtime_run_source_failed",
            result);
      }

      return result;
    }

    if (!source.executable())
    {
      auto error = make_runtime_error(
          RuntimeErrorCode::UnsupportedSourceType,
          "source file is not executable: " + source.path);

      result = RuntimeResult::failure(error, 1);
      result.mark_finished();

      if (loop_)
      {
        loop_->diagnostics().record_result(
            "runtime_run_source_failed",
            result);
      }

      return result;
    }

    result = RuntimeResult::success(
        "source validated successfully");

    result.mark_finished();

    if (loop_)
    {
      loop_->diagnostics().record(
          DiagnosticLevel::Info,
          "runtime_run_source",
          "source validated successfully");
    }

    return result;
  }

  Error Runtime::post(TaskFunction task)
  {
    if (!loop_)
    {
      return make_runtime_error(
          RuntimeErrorCode::InvalidState,
          "runtime loop is not initialized");
    }

    return loop_->post(std::move(task));
  }

  RuntimeResult Runtime::run_task(Task task)
  {
    return task.run();
  }

  Result<Manifest> Runtime::load_manifest(
      const std::string &path) const
  {
    return Manifest::load(path);
  }

  Process Runtime::process() const
  {
    return Process(config_);
  }

  Diagnostics &Runtime::diagnostics() noexcept
  {
    return loop_->diagnostics();
  }

  const Diagnostics &Runtime::diagnostics() const noexcept
  {
    return loop_->diagnostics();
  }

  const RuntimeConfig &Runtime::config() const noexcept
  {
    return config_;
  }

  const ModuleResolver &Runtime::module_resolver() const noexcept
  {
    return module_resolver_;
  }

  RuntimeLoop &Runtime::loop() noexcept
  {
    return *loop_;
  }

  const RuntimeLoop &Runtime::loop() const noexcept
  {
    return *loop_;
  }

} // namespace kordex::runtime
