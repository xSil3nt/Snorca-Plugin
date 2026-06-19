#include "orca/plugin/PluginABI.hpp"
#include "orca/plugin/PluginContext.hpp"

using namespace Slic3r;

static void register_round_prime_tower(PluginContext &ctx)
{
    PluginEnumExtension enum_ext;
    enum_ext.option_key  = "wipe_tower_wall_type";
    enum_ext.enum_values = {"round"};
    enum_ext.enum_labels = {"Round"};
    ctx.config_schema().extend_enum(enum_ext);

    PluginGUIOptionLine gui_line;
    gui_line.optgroup_key = "multimaterial_settings_prime_tower";
    gui_line.option_key   = "wipe_tower_wall_type";
    gui_line.doc_path     = "multimaterial_settings_prime_tower#wall-type";
    ctx.gui_support().register_option_line(gui_line);
}

ORCA_PLUGIN_API Slic3r::OrcaPluginDescriptor *orca_plugin_main()
{
    static OrcaPluginDescriptor descriptor{
        "orca.round_prime_tower",
        "Round Prime Tower",
        "1.0.0",
        ORCA_PLUGIN_ABI_VERSION,
        register_round_prime_tower,
    };
    return &descriptor;
}
