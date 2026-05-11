# Kordex Runtime

Core runtime foundation for Kordex.

`kordex-runtime` provides the base execution model used by the Kordex ecosystem.
It does not execute JavaScript directly. Instead, it provides the runtime configuration, source loading, module resolution, lifecycle management, permissions, diagnostics, tasks, timers, process facade, and manifest support used by higher layers such as `kordex-bindings`, `kordex-std`, and `kordex-cli`.

## Role

`kordex-runtime` is the foundation layer.

It is responsible for:

- runtime options
- normalized runtime configuration
- runtime permissions
- lifecycle state
- source file loading
- module identifier parsing
- module resolution
- project manifest loading
- diagnostics
- cancellation
- runtime loop
- tasks
- timers
- process access facade

The runtime module stays engine-independent.
JavaScript execution is handled by `kordex-bindings`.

## Public headers

```txt
include/kordex/runtime/
├── Cancellation.hpp
├── Clock.hpp
├── Diagnostics.hpp
├── Error.hpp
├── Manifest.hpp
├── ModuleId.hpp
├── ModuleResolver.hpp
├── Process.hpp
├── Result.hpp
├── Runtime.hpp
├── RuntimeConfig.hpp
├── RuntimeLoop.hpp
├── RuntimeOptions.hpp
├── RuntimeResult.hpp
├── RuntimeState.hpp
├── SourceFile.hpp
├── Task.hpp
├── Timer.hpp
└── Version.hpp
```

## Features

- Runtime lifecycle management
- Runtime configuration normalization
- Runtime permission model
- Source file loading
- JavaScript, TypeScript, and JSON source detection
- Module specifier parsing
- Relative module resolution
- Extension resolution
- Directory index resolution
- Manifest loading from `kordex.json` or `package.json`
- Runtime diagnostics
- Cancellation support
- Runtime loop built on Vix async/runtime foundations
- Task execution helpers
- Timer facade
- Process facade with permission checks
- Structured runtime results

## Runtime options

`RuntimeOptions` represents caller-provided settings.

```cpp
kordex::runtime::RuntimeOptions options;

options.mode = kordex::runtime::RuntimeMode::Development;
options.permission_mode = kordex::runtime::PermissionMode::Strict;

options.allow_fs = true;
options.allow_net = false;
options.allow_process = false;
options.allow_env = true;

options.diagnostics = true;
options.debug = false;
```

Factory helpers are available:

```cpp
auto dev = kordex::runtime::RuntimeOptions::development();
auto prod = kordex::runtime::RuntimeOptions::production();
auto test = kordex::runtime::RuntimeOptions::test();
```

## Runtime config

`RuntimeConfig` is the normalized internal configuration.

```cpp
auto config_result =
    kordex::runtime::RuntimeConfig::from_options(options);

if (!config_result)
{
  return 1;
}

auto config = config_result.value();
```

`RuntimeConfig` validates:

- runtime mode
- permission mode
- worker count
- cache directory
- working directory
- timeout
- capability permissions

## Permissions

The runtime stores capability flags:

```cpp
bool allow_fs;
bool allow_net;
bool allow_process;
bool allow_env;
```

These flags are later used by higher layers.

For example, the CLI converts `RuntimeConfig` into `StdOptions` so that sensitive modules are only installed when allowed:

```txt
RuntimeConfig.allow_fs      -> StdOptions.enable_fs
RuntimeConfig.allow_env     -> StdOptions.enable_env
RuntimeConfig.allow_process -> StdOptions.enable_process
RuntimeConfig.allow_net     -> StdOptions.enable_http
```

The runtime itself can also enforce permissions in facades such as `Process`.

## Source files

`SourceFile` loads and validates source files from disk.

Supported types:

- `.js`
- `.mjs`
- `.cjs`
- `.ts`
- `.mts`
- `.cts`
- `.json`

Example:

```cpp
auto source = kordex::runtime::SourceFile::load("src/main.ts");

if (!source)
{
  return 1;
}

if (source.value().is_typescript())
{
  // Pass to bindings TypeScript loader later.
}
```

Source type detection:

```cpp
auto type = kordex::runtime::detect_source_type("main.js");
```

Executable source types:

- JavaScript
- TypeScript

JSON is loadable but not directly executable by the runtime.

## Module identifiers

`ModuleId` parses module specifiers.

Examples:

```txt
./app.js        -> Relative
../lib/mod.js   -> Relative
/app/main.js    -> Absolute
kordex:fs       -> Builtin
react           -> Package
@scope/pkg      -> Package
```

Usage:

```cpp
auto module_id = kordex::runtime::ModuleId::parse("./lib/message.js");

if (!module_id)
{
  return 1;
}

if (module_id.value().is_relative())
{
  // Resolve through ModuleResolver.
}
```

## Module resolver

`ModuleResolver` resolves file module paths.

It supports:

- relative paths
- absolute paths
- extension resolution
- directory index resolution
- builtin detection
- package detection foundation

Typical resolution:

```cpp
kordex::runtime::ModuleResolverOptions options;
options.base_dir = "src";
options.resolve_extensions = true;
options.resolve_index = true;
options.allow_builtins = true;
options.allow_packages = false;

kordex::runtime::ModuleResolver resolver(options);

auto resolved = resolver.resolve("./lib/message");

if (!resolved)
{
  return 1;
}
```

This can resolve:

```txt
./lib/message
-> ./lib/message.js
-> ./lib/message.ts
-> ./lib/message/index.js
```

The bindings module uses this resolver when building the module graph for JavaScript execution.

## Manifest loading

`Manifest` loads project metadata from:

- `kordex.json`
- `package.json`

Example:

```cpp
auto manifest = kordex::runtime::Manifest::load("kordex.json");

if (!manifest)
{
  return 1;
}

if (manifest.value().has_entry())
{
  auto entry = manifest.value().entry;
}
```

Supported manifest fields include:

- name
- version
- entry
- environment
- scripts
- raw JSON

## Runtime lifecycle

`RuntimeState` tracks lifecycle state.

States:

- Created
- Starting
- Running
- Stopping
- Stopped
- Failed

Example:

```cpp
kordex::runtime::RuntimeState state;

auto error = state.mark_starting();
if (error)
{
  return 1;
}

state.mark_running();
```

## Runtime facade

`Runtime` is the public runtime facade.

Example:

```cpp
kordex::runtime::Runtime runtime;

auto error = runtime.start();
if (error)
{
  return 1;
}

auto source = runtime.load_source("main.js");

runtime.shutdown();
```

The current runtime facade can:

- start and stop the runtime loop
- load source files
- resolve modules
- load manifests
- submit tasks
- run simple runtime tasks
- expose diagnostics
- expose process facade

JavaScript execution is intentionally delegated to `kordex-bindings`.

## Runtime results

`RuntimeResult` represents the result of a runtime operation.

Statuses:

- Success
- Failed
- Cancelled
- Timeout

Example:

```cpp
auto result = kordex::runtime::RuntimeResult::success("done");

if (result.succeeded())
{
  // ok
}
```

Failure:

```cpp
auto result = kordex::runtime::RuntimeResult::failure(
    kordex::runtime::make_runtime_error(
        kordex::runtime::RuntimeErrorCode::InternalError,
        "runtime failed"));
```

## Diagnostics

Diagnostics provide structured runtime events.

```cpp
kordex::runtime::Diagnostics diagnostics;

diagnostics.record(
    kordex::runtime::DiagnosticLevel::Info,
    "runtime_start",
    "Runtime started");
```

Diagnostics can be converted to JSON:

```cpp
auto json = diagnostics.to_json();
```

Diagnostic events include:

- level
- name
- message
- path
- timestamp
- details

## Cancellation

The runtime uses Vix async cancellation primitives through Kordex aliases.

```cpp
kordex::runtime::Cancellation cancellation;

auto token = cancellation.token();

cancellation.request_cancel();

if (cancellation.is_cancelled())
{
  // cancelled
}
```

## Runtime loop

`RuntimeLoop` owns the lower-level execution services:

- Vix async io_context
- Vix scheduler
- runtime state
- cancellation
- diagnostics
- worker runtime

Example:

```cpp
kordex::runtime::RuntimeLoop loop;

auto error = loop.start();
if (error)
{
  return 1;
}

loop.post([] {
  // task
});

loop.shutdown();
```

## Tasks

`Task` wraps a callable and tracks its status.

```cpp
kordex::runtime::Task task(
    [] {
      // work
    },
    kordex::runtime::TaskOptions{
        .name = "example-task",
        .critical = false,
        .may_block = false});

auto result = task.run();
```

Task statuses:

- Pending
- Running
- Completed
- Failed
- Cancelled

## Timers

`Timer` wraps Vix async timers.

```cpp
kordex::runtime::Timer timer(loop.io_context());

auto task = timer.sleep_for(
    kordex::runtime::Clock::seconds(1));
```

Timer statuses:

- Idle
- Waiting
- Fired
- Cancelled
- Failed

## Process facade

`Process` wraps the Vix process module and checks runtime permissions before process execution.

```cpp
kordex::runtime::RuntimeOptions options;
options.allow_process = true;

auto config = kordex::runtime::RuntimeConfig::from_options(options);
if (!config)
{
  return 1;
}

kordex::runtime::Process process(config.value());
```

Command example:

```cpp
kordex::runtime::ProcessCommand command;
command.program = "echo";
command.args = {"hello"};

auto output = process.output(command);
```

If `allow_process` is false, process operations fail with a permission error.

## Clock

`Clock` wraps Vix time utilities.

```cpp
auto now = kordex::runtime::Clock::now();
auto start = kordex::runtime::Clock::steady_now();

// work

auto elapsed = kordex::runtime::Clock::elapsed_since(start);
```

Duration helpers:

```cpp
auto ms = kordex::runtime::Clock::milliseconds(500);
auto sec = kordex::runtime::Clock::seconds(5);
auto min = kordex::runtime::Clock::minutes(1);
```

## Error handling

The runtime uses structured errors and `Result<T>`.

```cpp
auto source = kordex::runtime::SourceFile::load("missing.js");

if (!source)
{
  auto message = source.error().message();
}
```

Recoverable failures should return:

```cpp
kordex::runtime::Result<T>
```

instead of throwing exceptions.

## Pipeline role

When the user runs:

```bash
kordex run src/main.ts
```

The runtime layer participates in this pipeline:

```txt
CLI parses command
-> RuntimeOptions created
-> RuntimeConfig normalized
-> SourceFile loads src/main.ts
-> ModuleResolver resolves imports
-> Bindings executes through QuickJS
-> RuntimeResult or ScriptResult returned
```

The runtime does not own JavaScript execution.
It owns the execution foundation.

## Build from source

From the module directory:

```bash
vix build \
  --preset dev-ninja
```

With tests:

```bash
vix build \
  --preset dev-ninja \
  -- \
  -DKORDEX_RUNTIME_BUILD_TESTS=ON

vix tests -- --output-on-failure
```

With examples:

```bash
vix build \
  --preset dev-ninja \
  -- \
  -DKORDEX_RUNTIME_BUILD_EXAMPLES=ON
```

## Tests

The runtime tests should cover:

- runtime options
- runtime config
- source file loading
- source type detection
- module id parsing
- module resolution
- manifest loading
- runtime lifecycle
- runtime loop
- diagnostics
- cancellation
- tasks
- timers
- process permission checks

Run all tests:

```bash
vix tests
```

Run tests with raw CTest failure output:

```bash
vix tests -- --output-on-failure
```

Run one test by name or regex:

```bash
vix tests -R runtime
```

## Examples

Example programs can demonstrate:

- creating a runtime
- loading a source file
- resolving a module
- loading a manifest
- posting a task
- using timers
- using cancellation
- using process facade with permissions

Run a built runtime example:

```bash
vix run examples/create_runtime.cpp
```

Run another example:

```bash
vix run examples/run_file.cpp
```

## Current limitations

- JavaScript execution is not implemented in `kordex-runtime`
- Package resolution is only a foundation at this layer
- Full Node-style package resolution is handled later by higher layers
- TypeScript transpilation is handled by `kordex-bindings`
- Standard modules are provided by `kordex-std`
- User-facing commands are provided by `kordex-cli`

## Relationship with other Kordex modules

```txt
kordex-runtime
  -> foundation layer

kordex-bindings
  -> JavaScript engine bridge

kordex-std
  -> native modules exposed to scripts

kordex-cli
  -> user-facing command-line interface
```

The runtime is intentionally small, stable, and engine-independent.

## License

MIT License.
