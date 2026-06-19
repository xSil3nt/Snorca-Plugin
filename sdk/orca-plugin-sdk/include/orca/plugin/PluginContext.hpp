#pragma once

#include "ConfigSchemaRegistry.hpp"
#include "GCodeTransformRegistry.hpp"
#include "InfillProviderRegistry.hpp"
#include "IWallGenerator.hpp"
#include "IWipeTowerShape.hpp"
#include "PipelineStageRegistry.hpp"
#include "PluginGUISupport.hpp"
#include "SlicingHookBus.hpp"

#include <string>

namespace Slic3r {

class PluginContext
{
public:
    PluginContext(ConfigSchemaRegistry &config_schema,
                  WipeTowerShapeRegistry &wipe_tower_shapes,
                  WallGeneratorRegistry &wall_generators,
                  InfillProviderRegistry &infill_providers,
                  PipelineStageRegistry &pipeline_stages,
                  GCodeTransformRegistry &gcode_transforms,
                  SlicingHookBus &slicing_hooks,
                  PluginGUISupport &gui_support)
        : m_config_schema(config_schema)
        , m_wipe_tower_shapes(wipe_tower_shapes)
        , m_wall_generators(wall_generators)
        , m_infill_providers(infill_providers)
        , m_pipeline_stages(pipeline_stages)
        , m_gcode_transforms(gcode_transforms)
        , m_slicing_hooks(slicing_hooks)
        , m_gui_support(gui_support)
    {}

    ConfigSchemaRegistry   &config_schema() { return m_config_schema; }
    WipeTowerShapeRegistry &wipe_tower_shapes() { return m_wipe_tower_shapes; }
    WallGeneratorRegistry  &wall_generators() { return m_wall_generators; }
    InfillProviderRegistry &infill_providers() { return m_infill_providers; }
    PipelineStageRegistry  &pipeline_stages() { return m_pipeline_stages; }
    GCodeTransformRegistry &gcode_transforms() { return m_gcode_transforms; }
    SlicingHookBus         &slicing_hooks() { return m_slicing_hooks; }
    PluginGUISupport       &gui_support() { return m_gui_support; }

    void set_plugin_id(const std::string &id) { m_plugin_id = id; }
    const std::string &plugin_id() const { return m_plugin_id; }

private:
    std::string            m_plugin_id;
    ConfigSchemaRegistry   &m_config_schema;
    WipeTowerShapeRegistry &m_wipe_tower_shapes;
    WallGeneratorRegistry  &m_wall_generators;
    InfillProviderRegistry &m_infill_providers;
    PipelineStageRegistry  &m_pipeline_stages;
    GCodeTransformRegistry &m_gcode_transforms;
    SlicingHookBus         &m_slicing_hooks;
    PluginGUISupport       &m_gui_support;
};

} // namespace Slic3r
