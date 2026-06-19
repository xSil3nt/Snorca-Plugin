# Orca Plugin SDK

The Orca Plugin SDK lets you build native shared-library plugins for Snapmaker Orca.

## Requirements

- Same compiler/toolchain as the host Snapmaker Orca build
- Matching `ORCA_PLUGIN_ABI_VERSION` (currently **1**)
- CMake 3.13+

## Quick start

```cmake
find_package(OrcaPluginSDK REQUIRED)
add_library(my_plugin SHARED my_plugin.cpp)
target_link_libraries(my_plugin PRIVATE OrcaPluginSDK libslic3r)
```

Each plugin must export:

```cpp
ORCA_PLUGIN_API Slic3r::OrcaPluginDescriptor *orca_plugin_main();
```

Package your plugin as a directory or `.orcaplugin` zip containing:

- `manifest.json`
- platform shared library (`orca_myplugin.dll` / `liborca_myplugin.so` / `liborca_myplugin.dylib`)

## Discovery

Plugins are loaded from:

1. `--load-plugin <path>` CLI flag (directory or `.orcaplugin`)
2. `<datadir>/plugins/` subdirectories

## Extension points

| Registry | Purpose |
|----------|---------|
| `ConfigSchemaRegistry` | Runtime print/filament/printer settings |
| `WipeTowerShapeRegistry` | Prime tower wall geometry (`IWipeTowerShape`) |
| `WallGeneratorRegistry` | Perimeter generators (`IWallGenerator`) |
| `InfillProviderRegistry` | Custom infill pattern factories |
| `SlicingHookBus` | Hooks at print/object step boundaries and after G-code export |
| `PluginGUISupport` | Sidebar option lines and visibility rules |

## Examples

See `plugins/examples/noop` and `plugins/examples/round_prime_tower`.

## ABI policy

Plugins must be rebuilt when `ORCA_PLUGIN_ABI_VERSION` or the host minor version changes. The loader rejects mismatched ABI versions at load time.
