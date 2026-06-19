#include "PluginGUISupport.hpp"

namespace Slic3r {

void PluginGUISupport::register_option_line(const PluginGUIOptionLine &line)
{
    m_option_lines.push_back(line);
}

void PluginGUISupport::register_visibility_rule(const PluginGUIVisibilityRule &rule)
{
    m_visibility_rules.push_back(rule);
}

} // namespace Slic3r
