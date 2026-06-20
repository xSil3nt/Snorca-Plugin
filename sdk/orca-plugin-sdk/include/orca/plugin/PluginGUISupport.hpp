#pragma once

#include <functional>
#include <string>
#include <vector>

namespace Slic3r {

struct PluginGUIOptionLine
{
    std::string optgroup_key;
    std::string option_key;
    std::string doc_path;
};

struct PluginGUIVisibilityRule
{
    std::string option_key;
    std::string depends_on_option;
    std::string depends_on_value;
};

class PluginGUISupport
{
public:
    void register_option_line(const PluginGUIOptionLine &line) { m_option_lines.push_back(line); }
    void register_visibility_rule(const PluginGUIVisibilityRule &rule) { m_visibility_rules.push_back(rule); }

    const std::vector<PluginGUIOptionLine>     &option_lines() const { return m_option_lines; }
    const std::vector<PluginGUIVisibilityRule> &visibility_rules() const { return m_visibility_rules; }

private:
    std::vector<PluginGUIOptionLine>     m_option_lines;
    std::vector<PluginGUIVisibilityRule> m_visibility_rules;
};

} // namespace Slic3r
