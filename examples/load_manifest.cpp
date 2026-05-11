/**
 *
 *  @file load_manifest.cpp
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

#include <iostream>
#include <string>

#include <kordex/runtime/Manifest.hpp>

namespace
{
  int run_load_manifest_example(const std::string &path)
  {
    auto manifest = kordex::runtime::Manifest::load(path);

    if (!manifest)
    {
      std::cerr << "failed to load manifest: "
                << manifest.error().message()
                << '\n';

      return 1;
    }

    const auto &value = manifest.value();

    std::cout << "manifest path: " << value.path << '\n';
    std::cout << "manifest type: "
              << kordex::runtime::to_string(value.type)
              << '\n';

    if (value.has_name())
    {
      std::cout << "name: " << value.name << '\n';
    }

    if (value.has_version())
    {
      std::cout << "version: " << value.version << '\n';
    }

    if (value.has_entry())
    {
      std::cout << "entry: " << value.entry << '\n';
    }

    if (!value.environment.empty())
    {
      std::cout << "environment: " << value.environment << '\n';
    }

    std::cout << "scripts: " << value.scripts.size() << '\n';

    return 0;
  }
} // namespace

int main(int argc, char **argv)
{
  const std::string path = argc > 1 ? argv[1] : "package.json";

  return run_load_manifest_example(path);
}
