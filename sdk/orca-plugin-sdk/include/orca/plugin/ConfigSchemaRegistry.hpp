#pragma once

#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace Slic3r {

enum class PluginPresetType {
    Print,
    Filament,
    Printer,
    PhysicalPrinter,
};

struct PluginEnumExtension
{
    std::string              option_key;
    std::vector<std::string> enum_values;
    std::vector<std::string> enum_labels;
};

struct PluginConfigOptionDef
{
    std::string              opt_key;
    int                      type{0};
    std::vector<std::string> enum_values;
    std::vector<std::string> enum_labels;
    std::string              label;
    std::string              tooltip;
    std::string              category;
    std::string              sidetext;
    float                    min_float{0.f};
    float                    max_float{0.f};
    float                    default_float{0.f};
    int                      default_int{0};
    std::string              default_string;
};

struct PluginConfigRegistration
{
    PluginConfigOptionDef def;
    PluginPresetType      preset_type{PluginPresetType::Print};
    std::vector<int>      invalidate_print_steps;
    std::vector<int>      invalidate_object_steps;
};

class ConfigSchemaRegistry
{
public:
    void register_option(const PluginConfigRegistration &registration)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_registrations.push_back(registration);
        m_applied = false;
    }

    void extend_enum(const PluginEnumExtension &extension)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_enum_extensions.push_back(extension);
        m_applied = false;
    }

    void apply_to_print_config_def();
    void register_preset_options();
    void merge_invalidation(const std::vector<std::string> &opt_keys,
                            std::vector<int> &print_steps,
                            std::vector<int> &object_steps) const;

    const std::vector<PluginConfigRegistration> &registrations() const { return m_registrations; }
    const std::vector<PluginEnumExtension>      &enum_extensions() const { return m_enum_extensions; }

private:
    mutable std::mutex                    m_mutex;
    std::vector<PluginConfigRegistration> m_registrations;
    std::vector<PluginEnumExtension>      m_enum_extensions;
    bool                                  m_applied{false};
};

} // namespace Slic3r
