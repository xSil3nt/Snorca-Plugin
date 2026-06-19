#include "GUIContributionRegistry.hpp"

#include "libslic3r/Config.hpp"
#include "libslic3r/Plugin/PluginManager.hpp"
#include "libslic3r/Plugin/WipeTowerShapeRegistry.hpp"
#include "libslic3r/PrintConfig.hpp"

namespace Slic3r {

namespace {

std::string config_option_key_string(const DynamicPrintConfig *config, const std::string &option_key)
{
    if (config == nullptr || !config->has(option_key))
        return {};
    const ConfigOption *opt = config->option(option_key);
    if (opt == nullptr)
        return {};

    switch (opt->type()) {
    case coString: return config->opt_string(option_key);
    case coEnum: {
        std::string key = PluginManager::instance().enum_key_for_value(option_key, opt->getInt());
        if (!key.empty())
            return key;
        if (option_key == "wipe_tower_wall_type")
            return wipe_tower_wall_type_key(opt->getInt());
        return std::to_string(opt->getInt());
    }
    default:
        if (opt->is_scalar())
            return std::to_string(opt->getInt());
        return {};
    }
}

} // namespace

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
        registry.register_visibility_rule({rule.option_key, [rule](const DynamicPrintConfig *config) {
            if (config == nullptr)
                return false;
            const std::string current = config_option_key_string(config, rule.depends_on_option);
            return !current.empty() && current == rule.depends_on_value;
        }});
    }
}

} // namespace Slic3r
