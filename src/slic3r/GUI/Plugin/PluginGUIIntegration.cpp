#include "GUIContributionRegistry.hpp"

#include "libslic3r/Plugin/PluginManager.hpp"
#include "libslic3r/PrintConfig.hpp"

namespace Slic3r {

void sync_plugin_gui_contributions_from_manager()
{
    auto &registry = GUIContributionRegistry::instance();
    const PluginGUISupport &gui = PluginManager::instance().gui_support();

    for (const PluginGUIOptionLine &line : gui.option_lines()) {
        GUIOptionContribution contribution;
        contribution.optgroup_key = line.optgroup_key;
        contribution.option_key   = line.option_key;
        contribution.doc_path     = line.doc_path;
        registry.register_option_line(contribution);
    }

    for (const PluginGUIVisibilityRule &rule : gui.visibility_rules()) {
        if (rule.depends_on_option == "wipe_tower_wall_type") {
            registry.register_visibility_rule({rule.option_key, [rule](const DynamicPrintConfig *config) {
                if (config == nullptr)
                    return false;
                if (!config->has("wipe_tower_wall_type"))
                    return false;
                const ConfigOption *opt = config->option("wipe_tower_wall_type");
                return opt && opt->serialize() == rule.depends_on_value;
            }});
        }
    }
}

} // namespace Slic3r
