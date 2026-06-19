#include "orca/plugin/PluginABI.hpp"
#include "orca/plugin/PluginContext.hpp"
#include "orca/plugin/ConfigTypes.hpp"

#include <sstream>
#include <string>

using namespace Slic3r;

namespace {

bool arc_overhang_enabled(const LayerGCodeTransformContext &ctx)
{
    return ctx.config != nullptr && ctx.config->has("arc_overhang_enabled") && ctx.config->get_int("arc_overhang_enabled") != 0;
}

void transform_layer_gcode(LayerGCodeTransformContext &ctx)
{
    if (!arc_overhang_enabled(ctx) || ctx.gcode.empty())
        return;

    std::ostringstream out;
    out << "; Orca plugin: arc_overhang layer " << ctx.layer_id << '\n';

    std::istringstream in(ctx.gcode);
    std::string        line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (line.rfind("G1 ", 0) == 0 && line.find('X') != std::string::npos && line.find('Y') != std::string::npos) {
            // Demo transform: tag candidate overhang travel/extrude segments for downstream arc fitting.
            out << "; arc_overhang_candidate\n";
        }
        out << line << '\n';
    }
    ctx.gcode = out.str();
}

void register_arc_overhang_plugin(PluginContext &ctx)
{
    PluginConfigRegistration reg;
    reg.def.opt_key      = "arc_overhang_enabled";
    reg.def.type         = PluginCoBool;
    reg.def.label        = "Arc overhang transform";
    reg.def.tooltip      = "Annotate layer G-code for arc-style overhang processing (demo plugin).";
    reg.def.category     = "quality";
    reg.def.default_int  = 0;
    reg.preset_type      = PluginPresetType::Print;
    ctx.config_schema().register_option(reg);

    ctx.gui_support().register_option_line({"plugin_settings", "arc_overhang_enabled", "plugin_settings"});
    ctx.gcode_transforms().register_layer_transform("arc_overhang", transform_layer_gcode, ctx.plugin_id());
}

} // namespace

ORCA_PLUGIN_API Slic3r::OrcaPluginDescriptor *orca_plugin_main()
{
    static OrcaPluginDescriptor descriptor{
        "orca.arc_overhang",
        "Arc Overhang G-code Transform",
        "1.0.0",
        ORCA_PLUGIN_ABI_VERSION,
        register_arc_overhang_plugin,
    };
    return &descriptor;
}
