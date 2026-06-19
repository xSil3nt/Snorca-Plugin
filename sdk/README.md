# Orca Plugin SDK

The Orca Plugin SDK lets you build native shared-library plugins for Snapmaker Orca without linking against `libslic3r`.

## Requirements

- Same compiler/toolchain as the host Snapmaker Orca build
- Matching `ORCA_PLUGIN_ABI_VERSION` (currently **2**)
- CMake 3.13+
- Eigen3 (pulled in transitively via `OrcaPluginSDK`)

## Quick start

```cmake
find_package(OrcaPluginSDK REQUIRED)
add_library(my_plugin SHARED my_plugin.cpp)
target_link_libraries(my_plugin PRIVATE OrcaPluginSDK)
target_compile_definitions(my_plugin PRIVATE ORCA_PLUGIN_EXPORTS)
```

Each plugin must export:

```cpp
ORCA_PLUGIN_API Slic3r::OrcaPluginDescriptor *orca_plugin_main();
```

Package your plugin as a directory or `.orcaplugin` zip containing:

- `manifest.json` with `"abi_version": 2`
- platform shared library (`orca_myplugin.dll` / `liborca_myplugin.so` / `liborca_myplugin.dylib`)

## Discovery

Plugins are loaded from:

1. `--load-plugin <path>` CLI flag (directory or `.orcaplugin`)
2. `<datadir>/plugins/` subdirectories

## ABI v2 changes

- Standalone headers under `include/orca/plugin/` (no `libslic3r` source dependency)
- `get_infill_circle` removed from `IWipeTowerShape` (use Phase 2 path overrides instead)
- Plugin geometry types live in `Slic3r::PluginGeo` (`orca/plugin/geometry/`)
- `ConfigSchemaRegistry` invalidation steps use `int` lists (host maps to `PrintStep` internally)

## Extension points

| Registry | Purpose |
|----------|---------|
| `ConfigSchemaRegistry` | Runtime print/filament/printer settings |
| `WipeTowerShapeRegistry` | Prime tower wall geometry (`IWipeTowerShape`) |
| `WallGeneratorRegistry` | Perimeter generators (`IWallGenerator`, host-dev) |
| `InfillProviderRegistry` | Custom infill factories (host-dev) |
| `SlicingHookBus` | Hooks at print/object step boundaries and after G-code export |
| `PluginGUISupport` | Sidebar option lines and visibility rules |

## Wipe tower API (headers only in v2)

- `IWipeTowerPathWriter` — path emission surface for plugin overrides
- `WipeTowerTypes.hpp` — shared contexts and `WipeTowerHostServices`
- Optional virtual hooks on `IWipeTowerShape` (`generate_sparse_scaffold`, `generate_toolchange_wipe`, `generate_toolchange_ramming`, etc.) return `false` by default; the host delegates to plugins when they return `true`.

## Examples

See `plugins/examples/noop` and `plugins/examples/round_prime_tower`.

Build examples from the Orca tree:

```bash
cmake -S . -B build -DORCA_BUILD_EXAMPLE_PLUGINS=ON
cmake --build build --target orca_round_prime_tower
```

## Future extensions

The round prime tower example demonstrates wipe-tower plugins only. The SDK also exposes registries for broader features (documented here; deep APIs may require host rebuilds today):

- **Wall generators** (`IWallGenerator` / `WallGeneratorRegistry`): custom perimeter strategies such as wave or arc overhang walls. Register a generator key and wire it through host print settings when the GUI surface is extended.
- **Slicing hooks** (`SlicingHookBus`): `BeforePrintObjectStep` / `AfterPrintObjectStep` at slice, perimeter, and infill boundaries; `BeforePrintStep` / `AfterPrintStep` at print scope; `AfterGCodeExport` after export.
- **Infill providers** (`InfillProviderRegistry`): custom infill pattern factories (host-dev surface; standalone infill API deferred).

See [docs/wipe_tower_shape.md](docs/wipe_tower_shape.md) for a wipe-tower plugin tutorial.

## ABI policy

Plugins must be rebuilt when `ORCA_PLUGIN_ABI_VERSION` changes. The loader rejects mismatched ABI versions and manifest/descriptor `id` mismatches at load time.
