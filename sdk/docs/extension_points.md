# Orca Plugin SDK — Universal Extension Points (ABI v3)

ABI v3 introduces **five universal extension points**. After the host is built with plugin support, new feature *types* ship as plugins without further slicer changes.

## 1. PipelineStageRegistry

Register handlers for named pipeline stages. The host consults these stages automatically.

| Stage ID | When invoked | Example features |
|----------|--------------|------------------|
| `perimeters` | Wall generation | Wave walls, custom overhang perimeter logic |
| `infill` | Infill path generation | Custom sparse patterns |
| `support` | Support generation | Custom support styles |
| `overhang_filter` | Post-perimeter filtering | Overhang path rewrites |

```cpp
ctx.pipeline_stages().register_handler("infill", "my_pattern", [](PipelineStageContext &ctx) -> bool {
    // Read ctx.input_polygons, write ctx.output (PluginGeo::ExtrusionCollection)
    return true; // handled
});
```

Handlers registered for `perimeters` / `infill` are auto-synced to the legacy `WallGeneratorRegistry` / `InfillProviderRegistry` shims.

## 2. GCodeTransformRegistry

Per-layer G-code string transforms run inside the host `process_layers` pipeline (after layer G-code is generated, before cooling/fan filters).

```cpp
ctx.gcode_transforms().register_layer_transform("my_transform", [](LayerGCodeTransformContext &ctx) {
    // Mutate ctx.gcode, read ctx.config, ctx.layer_id
});
```

Use for: arc overhang post-processing, custom cooling annotations, M-code injection, speed overrides.

## 3. SlicingHookBus (mutable)

Hooks fire at every `PrintStep` and `PrintObjectStep`. ABI v3 adds `ISliceData*` to `SlicingHookContext` for reading and mutating per-layer extrusion collections across the DLL boundary.

```cpp
ctx.slicing_hooks().register_hook(SlicingHookPhase::AfterPrintObjectStep, [](const SlicingHookContext &ctx) {
    if (ctx.slice_data == nullptr) return;
    PluginGeo::ExtrusionCollection perimeters;
    ctx.slice_data->get_extrusion_collection(0, 0, PluginSliceCollectionKind::Perimeters, perimeters);
    // mutate and set back
});
```

## 4. AfterGCodeExport

Whole-file hook after G-code is written to disk. Context includes `gcode_path` and full `config`.

## 5. Config + Auto-GUI

```cpp
PluginConfigRegistration reg;
reg.def.opt_key = "my_option";
reg.def.type    = PluginCoBool;
reg.preset_type = PluginPresetType::Print;
ctx.config_schema().register_option(reg);

ctx.gui_support().register_option_line({"plugin_settings", "my_option", "plugin_settings"});
```

Registered options merge into `print_config_def`, persist in presets, and appear in the Process tab automatically (matched optgroup or fallback **Plugin settings** page).

## Typed APIs (still supported)

- `IWipeTowerShape` — full prime/wipe tower control (see `sdk/docs/wipe_tower_shape.md`)
- `WallGeneratorRegistry` / `InfillProviderRegistry` — legacy; prefer `PipelineStageRegistry`

## Which hook for which feature?

| Feature idea | Recommended hook |
|--------------|------------------|
| New infill pattern | `PipelineStageRegistry` (`infill`) |
| New wall strategy | `PipelineStageRegistry` (`perimeters`) |
| Arc/wave overhang G-code | `GCodeTransformRegistry` |
| Rewrite sliced paths | `SlicingHookBus` + `ISliceData` |
| Post-export G-code file edit | `AfterGCodeExport` |
| New setting + UI | `ConfigSchemaRegistry` + `PluginGUISupport` |
| Round/custom prime tower | `IWipeTowerShape` |

## Examples

- `plugins/examples/arc_overhang` — G-code transform demo
- `plugins/examples/custom_infill` — pipeline infill demo
- `plugins/examples/round_prime_tower` — wipe tower shape demo
