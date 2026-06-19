#ifndef slic3r_ConfigSchemaRegistry_hpp_
#define slic3r_ConfigSchemaRegistry_hpp_

#include "libslic3r/Config.hpp"
#include "libslic3r/Print.hpp"

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

struct PluginConfigRegistration
{
    ConfigOptionDef          def;
    PluginPresetType         preset_type{PluginPresetType::Print};
    std::vector<PrintStep>   invalidate_print_steps;
    std::vector<PrintObjectStep> invalidate_object_steps;
};

class ConfigSchemaRegistry
{
public:
    void register_option(const PluginConfigRegistration &registration);
    void extend_enum(const PluginEnumExtension &extension);

    void apply_to_print_config_def();
    void register_preset_options();
    void merge_invalidation(const std::vector<t_config_option_key> &opt_keys,
                            std::vector<PrintStep> &print_steps,
                            std::vector<PrintObjectStep> &object_steps) const;

    const std::vector<PluginConfigRegistration> &registrations() const { return m_registrations; }
    const std::vector<PluginEnumExtension> &enum_extensions() const { return m_enum_extensions; }

private:
    mutable std::mutex                       m_mutex;
    std::vector<PluginConfigRegistration>    m_registrations;
    std::vector<PluginEnumExtension>         m_enum_extensions;
    bool                                     m_applied{false};
};

} // namespace Slic3r

#endif // slic3r_ConfigSchemaRegistry_hpp_
