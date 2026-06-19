#include "GUIContributionRegistry.hpp"

#include "slic3r/GUI/ConfigManipulation.hpp"

namespace Slic3r {

GUIContributionRegistry &GUIContributionRegistry::instance()
{
    static GUIContributionRegistry registry;
    return registry;
}

void GUIContributionRegistry::register_option_line(const GUIOptionContribution &contribution)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_option_lines.push_back(contribution);
}

void GUIContributionRegistry::register_visibility_rule(const GUIVisibilityRule &rule)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_visibility_rules.push_back(rule);
}

void GUIContributionRegistry::apply_visibility_rules(ConfigManipulation *config_manipulation, const DynamicPrintConfig *config) const
{
    if (config_manipulation == nullptr || config == nullptr)
        return;
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const GUIVisibilityRule &rule : m_visibility_rules) {
        if (rule.visible)
            config_manipulation->toggle_line(rule.option_key.c_str(), rule.visible(config));
    }
}

} // namespace Slic3r
