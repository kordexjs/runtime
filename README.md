# Kordex Runtime

Core runtime module for Kordex.

Kordex Runtime provides the native foundation used to run JavaScript and TypeScript applications on top of Vix and Softadastra.

It is not the JavaScript engine itself.
It is the runtime layer that prepares execution, manages lifecycle, loads source files, resolves modules, controls permissions, schedules tasks, handles timers, exposes diagnostics, and connects later to native bindings.

## Role

Kordex is designed as:

```txt
Kordex      JavaScript/TypeScript runtime layer
Vix         C++ runtime foundation
Softadastra Durable local-first sync, WAL, offline-first guarantees
```

This repository contains the runtime module only.

It provides the stable C++ API that higher layers will use before adding JavaScript engine bindings.

## Current capabilities

The current runtime module includes:

- Runtime configuration
- Runtime lifecycle
- Runtime loop
- Source file loading
- Module identifier parsing
- Module resolution
- Manifest loading
- Diagnostics collection
- Cancellation tokens
- Task execution
- Timer helpers
- Process facade with permission checks
- Runtime result reporting

## Build

```bash
vix build -- -DCMAKE_BUILD_TYPE=Release
```
## Run tests

```bash
vix tests
```

## Examples

After building, examples are available in the build directory.

- `create_runtime`
- `run_file`
- `load_manifest`
- `spawn_process`
- `timer`

Example:

```bash
vix run ./build/examples/create_runtime
```

or with Ninja:

```bash
vix run ./build-ninja/examples/create_runtime
```

## Basic usage

```cpp
#include <kordex/runtime/Runtime.hpp>

int main()
{
  kordex::runtime::Runtime runtime =
      kordex::runtime::Runtime::development();

  auto error = runtime.start();
  if (error)
  {
    return 1;
  }

  auto result = runtime.run_file("app.js");

  runtime.shutdown();

  return result.succeeded() ? 0 : 1;
}
```

## Runtime configuration

```cpp
kordex::runtime::RuntimeOptions options =
    kordex::runtime::RuntimeOptions::development();

options.workers = 2;
options.diagnostics = true;
options.allow_process = false;

auto config = kordex::runtime::RuntimeConfig::from_options(options);
```

## Source files

```cpp
auto source = kordex::runtime::SourceFile::load("src/main.js");

if (!source)
{
  return 1;
}

if (source.value().executable())
{
  // JavaScript or TypeScript source
}
```

Supported source types:

- `.js`
- `.mjs`
- `.cjs`
- `.ts`
- `.mts`
- `.cts`
- `.json`

JSON files can be loaded and resolved, but they are not executable.

## Module resolution

```cpp
kordex::runtime::ModuleResolver resolver;

auto module = resolver.resolve("./src/main.js");
```

Supported module identifiers:

```
./app.js       relative file
../lib/mod.js  relative file
/app/main.js   absolute file
kordex:fs      builtin module
react          package module
@scope/pkg     scoped package module
```

Package resolution is currently represented as metadata. Full package loading will come later.

## Process access

Process execution is controlled by runtime configuration.

```cpp
kordex::runtime::RuntimeOptions options =
    kordex::runtime::RuntimeOptions::development();

options.allow_process = true;

auto config = kordex::runtime::RuntimeConfig::from_options(options);

kordex::runtime::Process process(config.value());
```

Run a command:

```cpp
kordex::runtime::ProcessCommand command;
command.program = "printf";
command.args = {"hello from Kordex\n"};
command.options.stdout_mode = kordex::runtime::ProcessStreamMode::Pipe;
command.options.stderr_mode = kordex::runtime::ProcessStreamMode::Pipe;

auto output = process.output(command);
```

## Timers

```cpp
kordex::runtime::Timer timer(context);

auto error = co_await timer.sleep_for(
    kordex::runtime::Duration::milliseconds(100));
```

Timers use the Vix async runtime internally.

## Tasks

```cpp
kordex::runtime::Task task([](){
      // work
});

auto result = task.run();
```

Tasks can also be adapted to Vix runtime tasks.

## Diagnostics

```cpp
runtime.diagnostics().record(
    kordex::runtime::DiagnosticLevel::Info,
    "runtime_started",
    "runtime started successfully");
```

Diagnostics are designed to make runtime behavior observable and debuggable.

## Design goals

Kordex Runtime follows a few clear rules:

- Simple public API
- Explicit runtime state
- No hidden process permissions
- Structured errors
- Deterministic module resolution
- Observable runtime behavior
- Native C++ foundation first
- JavaScript bindings later

## Current status

This module is an early runtime foundation.

It validates the core architecture before adding:

- JavaScript engine bindings
- TypeScript loading strategy
- standard library bindings
- package loading
- native module bridge
- CLI integration

## License

MIT License.

See LICENSE.
