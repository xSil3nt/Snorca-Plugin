# Orca Plugin SDK

The Orca Plugin SDK lets you build native shared-library plugins for Snapmaker Orca without linking against `libslic3r`.

## Requirements

- Same compiler/toolchain as the host Snapmaker Orca build
- Matching `ORCA_PLUGIN_ABI_VERSION` (currently **3**)
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

- `manifest.json` with `"abi_version": 3`
- platform shared library (`orca_myplugin.dll` / `liborca_myplugin.so` / `liborca_myplugin.dylib`)

## Discovery

Plugins are loaded from:

1. `--load-plugin <path>` CLI flag (directory or `.orcaplugin`)
2. `<datadir>/plugins/` subdirectories

## ABI v3 — universal extension points

ABI v3 replaces the per-feature treadmill with five universal hooks. See **[extension_points.md](docs/extension_points.md)** for the full guide.

| API | Purpose |
|-----|---------|
| `PipelineStageRegistry` | Generic stage handlers (`perimeters`, `infill`, `support`, …) |
| `GCodeTransformRegistry` | Per-layer G-code string transforms |
| `SlicingHookBus` + `ISliceData` | Mutable hooks at print/object steps |
| `AfterGCodeExport` | Whole-file G-code post-processing |
| `ConfigSchemaRegistry` + `PluginGUISupport` | Runtime settings with auto-rendered GUI |

Legacy typed registries (`WallGeneratorRegistry`, `InfillProviderRegistry`, `IWipeTowerShape`) remain for compatibility; new feature types should use the universal APIs above.

## SDK headers

- `orca/plugin/PluginABI.hpp` — ABI version and entry point
- `orca/plugin/PluginContext.hpp` — registration surface passed to `register_plugin`
- `orca/plugin/geometry/` — `Point`, `Polygon`, `ExtrusionTypes`
- `orca/plugin/SliceDataAccess.hpp` — `ISliceData`, `IPluginConfigReader`
- `orca/plugin/ConfigTypes.hpp` — `PluginCoBool`, `PluginCoFloat`, etc.

## Examples

| Plugin | Demonstrates |
|--------|--------------|
| `plugins/examples/noop` | Minimal plugin |
| `plugins/examples/round_prime_tower` | `IWipeTowerShape` |

Build examples from the Orca tree:

```bash
cmake -S . -B build -DORCA_BUILD_EXAMPLE_PLUGINS=ON
cmake --build build --target orca_round_prime_tower
```

## Wipe tower API

See [wipe_tower_shape.md](docs/wipe_tower_shape.md) for the full `IWipeTowerShape` tutorial.
