/**
 *
 *  @file Runtime.hpp
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

#ifndef KORDEX_RUNTIME_RUNTIME_HPP
#define KORDEX_RUNTIME_RUNTIME_HPP

#include <memory>
#include <string>

#include <kordex/runtime/Diagnostics.hpp>
#include <kordex/runtime/Manifest.hpp>
#include <kordex/runtime/ModuleResolver.hpp>
#include <kordex/runtime/Process.hpp>
#include <kordex/runtime/Result.hpp>
#include <kordex/runtime/RuntimeConfig.hpp>
#include <kordex/runtime/RuntimeLoop.hpp>
#include <kordex/runtime/RuntimeResult.hpp>
#include <kordex/runtime/SourceFile.hpp>
#include <kordex/runtime/Task.hpp>

namespace kordex::runtime
{
  /**
   * @class Runtime
   * @brief Public Kordex runtime facade.
   *
   * Runtime is the main developer-facing entry point.
   *
   * It combines:
   * - runtime configuration
   * - lifecycle management
   * - source file loading
   * - module resolution
   * - process access control
   * - task execution
   * - diagnostics
   */
  class Runtime
  {
  public:
    /**
     * @brief Construct a runtime using default configuration.
     */
    Runtime();

    /**
     * @brief Construct a runtime from explicit config.
     */
    explicit Runtime(RuntimeConfig config);

    Runtime(const Runtime &) = delete;
    Runtime &operator=(const Runtime &) = delete;

    Runtime(Runtime &&) noexcept;
    Runtime &operator=(Runtime &&) noexcept;

    /**
     * @brief Destroy the runtime and shutdown its loop if needed.
     */
    ~Runtime();

    /**
     * @brief Create a runtime from options.
     */
    [[nodiscard]] static Result<Runtime> from_options(
        const RuntimeOptions &options);

    /**
     * @brief Create a runtime configured for development.
     */
    [[nodiscard]] static Runtime development();

    /**
     * @brief Create a runtime configured for tests.
     */
    [[nodiscard]] static Runtime test();

    /**
     * @brief Start the runtime.
     */
    [[nodiscard]] Error start();

    /**
     * @brief Stop the runtime.
     */
    [[nodiscard]] Error stop();

    /**
     * @brief Shutdown the runtime.
     */
    [[nodiscard]] Error shutdown();

    /**
     * @brief Return true if the runtime is running.
     */
    [[nodiscard]] bool is_running() const noexcept;

    /**
     * @brief Return current lifecycle state.
     */
    [[nodiscard]] RuntimeLifecycleState state() const noexcept;

    /**
     * @brief Load a source file from disk.
     */
    [[nodiscard]] Result<SourceFile> load_source(
        const std::string &path) const;

    /**
     * @brief Resolve a module specifier.
     */
    [[nodiscard]] Result<ResolvedModule> resolve_module(
        const std::string &specifier) const;

    /**
     * @brief Run a source file.
     *
     * Current implementation validates and loads the file, then returns
     * a successful RuntimeResult. JavaScript execution is introduced later
     * by bindings.
     */
    [[nodiscard]] RuntimeResult run_file(
        const std::string &path);

    /**
     * @brief Run already loaded source content.
     *
     * Current implementation validates the source metadata and returns
     * a successful RuntimeResult. Execution is introduced later by bindings.
     */
    [[nodiscard]] RuntimeResult run_source(
        SourceFile source);

    /**
     * @brief Submit a simple task to the runtime loop.
     */
    [[nodiscard]] Error post(TaskFunction task);

    /**
     * @brief Run a task immediately on the current thread.
     */
    [[nodiscard]] RuntimeResult run_task(Task task);

    /**
     * @brief Load a manifest file.
     */
    [[nodiscard]] Result<Manifest> load_manifest(
        const std::string &path) const;

    /**
     * @brief Return the process facade.
     */
    [[nodiscard]] Process process() const;

    /**
     * @brief Return diagnostics collector.
     */
    [[nodiscard]] Diagnostics &diagnostics() noexcept;

    /**
     * @brief Return diagnostics collector.
     */
    [[nodiscard]] const Diagnostics &diagnostics() const noexcept;

    /**
     * @brief Return the runtime config.
     */
    [[nodiscard]] const RuntimeConfig &config() const noexcept;

    /**
     * @brief Return the module resolver.
     */
    [[nodiscard]] const ModuleResolver &module_resolver() const noexcept;

    /**
     * @brief Return the runtime loop.
     */
    [[nodiscard]] RuntimeLoop &loop() noexcept;

    /**
     * @brief Return the runtime loop.
     */
    [[nodiscard]] const RuntimeLoop &loop() const noexcept;

  private:
    explicit Runtime(RuntimeConfig config, bool start_loop);

    RuntimeConfig config_;
    ModuleResolver module_resolver_;
    std::unique_ptr<RuntimeLoop> loop_;
  };

} // namespace kordex::runtime

#endif // KORDEX_RUNTIME_RUNTIME_HPP
