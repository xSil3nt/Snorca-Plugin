# Wipe tower shape plugins

This tutorial walks through building a wipe-tower plugin like `orca.round_prime_tower` using only the Orca Plugin SDK (ABI v2).

## 1. Extend the wall type enum

```cpp
PluginEnumExtension enum_ext;
enum_ext.option_key  = "wipe_tower_wall_type";
enum_ext.enum_values = {"round"};
enum_ext.enum_labels = {"Round"};
ctx.config_schema().extend_enum(enum_ext);
```

List `wipe_tower_wall_type` under `config_keys` and `invalidation_keys` in `manifest.json`.

## 2. Register a shape provider

```cpp
ctx.wipe_tower_shapes().register_provider("round", []() {
    return std::make_unique<RoundPrimeTowerShape>();
});
```

The provider key must match the enum value string.

## 3. Implement `IWipeTowerShape`

| Method | Purpose |
|--------|---------|
| `generate_wall` | Return a `PluginGeo::Polygon` outline (host extrudes unless `includes_extruded_perimeter()` is true) |
| `generate_sparse_scaffold` | Optional: concentric/grid infill inside the tower; return `true` to skip built-in rectangular infill |
| `generate_toolchange_wipe` | Optional: custom purge paths during tool changes |
| `get_wipe_channel_width` | Optional: return `> 0` to override depth planning width (`width - 3*pw` default) |

Use `IWipeTowerPathWriter` for all extrusion/travel G-code. Access volume math via `WipeTowerHostServices` on the layer context (`ctx.layer.host`).

## 4. Geometry helpers

Include `orca/plugin/util/RoundTowerPaths.hpp` for chord clipping and concentric ring helpers, or implement your own path math in the plugin.

## 5. Package and load

```
my_plugin/
  manifest.json
  orca_my_plugin.dll
```

```powershell
snapmaker-orca.exe --load-plugin "D:\path\to\my_plugin"
```

## Beyond wipe towers

For features like wave/arc overhang walls, use `WallGeneratorRegistry` and `SlicingHookBus` (see [../README.md](../README.md)). Those surfaces are registered the same way; host integration for new enum keys may require a slicer update until generic GUI wiring lands.
