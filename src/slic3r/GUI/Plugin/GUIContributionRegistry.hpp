#ifndef slic3r_GUIContributionRegistry_hpp_
#define slic3r_GUIContributionRegistry_hpp_

#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace Slic3r {

class DynamicPrintConfig;

namespace GUI {
class ConfigManipulation;
}

struct GUIVisibilityRule
{
    std::string option_key;
    std::function<bool(const DynamicPrintConfig *config)> visible;
};

struct GUIOptionContribution
{
    std::string optgroup_key;
    std::string option_key;
    std::string doc_path;
    int         insert_after_line{-1};
};

class GUIContributionRegistry
{
public:
    static GUIContributionRegistry &instance();

    void register_option_line(const GUIOptionContribution &contribution);
    void register_visibility_rule(const GUIVisibilityRule &rule);
    void apply_visibility_rules(GUI::ConfigManipulation *config_manipulation, const DynamicPrintConfig *config) const;

    const std::vector<GUIOptionContribution> &option_lines() const { return m_option_lines; }

private:
    GUIContributionRegistry() = default;

    mutable std::mutex                m_mutex;
    std::vector<GUIOptionContribution> m_option_lines;
    std::vector<GUIVisibilityRule>       m_visibility_rules;
};

} // namespace Slic3r

#endif // slic3r_GUIContributionRegistry_hpp_
