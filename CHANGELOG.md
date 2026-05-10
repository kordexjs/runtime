# Changelog

All notable changes to Kordex Runtime will be documented in this file.

This project follows a simple release history format focused on clear runtime changes, public API additions, fixes, and compatibility notes.

## [Unreleased]

### Added

- Initial Kordex Runtime module structure.
- Public runtime facade through `kordex::runtime::Runtime`.
- Runtime lifecycle management with start, stop, shutdown, and state tracking.
- Runtime configuration through `RuntimeOptions` and `RuntimeConfig`.
- Structured runtime results through `RuntimeResult`.
- Structured error model through `Error`, `RuntimeErrorCode`, and `Result<T>`.
- Version API through `Version.hpp`.
- Clock helpers with `Timestamp`, `Duration`, and `Clock`.
- Manifest loading and validation.
- Diagnostics collection for runtime events and execution results.
- Cancellation controller and cancellation token support.
- Runtime loop built on top of the Vix runtime.
- Source file loading for JavaScript, TypeScript, and JSON files.
- Source type detection for:
  - `.js`
  - `.mjs`
  - `.cjs`
  - `.ts`
  - `.mts`
  - `.cts`
  - `.json`
- Module identifier parsing for:
  - relative imports
  - absolute imports
  - package imports
  - scoped package imports
  - builtin imports using `kordex:*`
- Module resolver with support for:
  - extension resolution
  - directory index resolution
  - builtin module metadata
  - package module metadata
- Process facade with runtime permission checks.
- Task wrapper with task status tracking and Vix runtime task adaptation.
- Timer facade built on top of the Vix async timer system.
- Public examples:
  - `create_runtime.cpp`
  - `run_file.cpp`
  - `load_manifest.cpp`
  - `spawn_process.cpp`
  - `timer.cpp`
- Unit tests for all initial runtime components.

### Notes

- JavaScript execution is not implemented yet.
- TypeScript transpilation is not implemented yet.
- Package loading is currently represented as metadata only.
- Builtin modules are resolved as metadata only.
- Native JavaScript engine bindings will be added in a later module.

## [0.1.0] - Planned

### Planned

- First public runtime foundation release.
- Stable C++ API for the Kordex runtime layer.
- Integration point for future JavaScript engine bindings.
- Initial documentation for runtime usage, examples, and build steps.
