#include "ConfigSchemaRegistry.hpp"

#include "libslic3r/Preset.hpp"
#include "libslic3r/PrintConfig.hpp"

#include <boost/log/trivial.hpp>

namespace Slic3r {

void ConfigSchemaRegistry::register_option(const PluginConfigRegistration &registration)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_registrations.push_back(registration);
    m_applied = false;
}

void ConfigSchemaRegistry::extend_enum(const PluginEnumExtension &extension)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_enum_extensions.push_back(extension);
    m_applied = false;
}

void ConfigSchemaRegistry::apply_to_print_config_def()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_applied)
        return;

    PrintConfigDef &def = const_cast<PrintConfigDef &>(print_config_def);

    for (const PluginEnumExtension &ext : m_enum_extensions) {
        ConfigOptionDef *optdef = def.get(ext.option_key);
        if (optdef == nullptr) {
            BOOST_LOG_TRIVIAL(warning) << "ConfigSchemaRegistry: cannot extend unknown enum option " << ext.option_key;
            continue;
        }
        for (size_t i = 0; i < ext.enum_values.size(); ++i) {
            if (std::find(optdef->enum_values.begin(), optdef->enum_values.end(), ext.enum_values[i]) == optdef->enum_values.end()) {
                optdef->enum_values.push_back(ext.enum_values[i]);
                if (i < ext.enum_labels.size())
                    optdef->enum_labels.push_back(ext.enum_labels[i]);
                else
                    optdef->enum_labels.push_back(ext.enum_values[i]);
            }
        }
    }

    for (const PluginConfigRegistration &reg : m_registrations) {
        if (def.has(reg.def.opt_key)) {
            BOOST_LOG_TRIVIAL(warning) << "ConfigSchemaRegistry: option already exists: " << reg.def.opt_key;
            continue;
        }
        def.options.emplace(reg.def.opt_key, reg.def);
    }

    m_applied = true;
}

void ConfigSchemaRegistry::register_preset_options()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const PluginConfigRegistration &reg : m_registrations) {
        if (reg.preset_type == PluginPresetType::Print)
            Preset::register_print_option(reg.def.opt_key);
    }
}

void ConfigSchemaRegistry::merge_invalidation(const std::vector<t_config_option_key> &opt_keys,
                                              std::vector<PrintStep> &print_steps,
                                              std::vector<PrintObjectStep> &object_steps) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const t_config_option_key &opt_key : opt_keys) {
        for (const PluginConfigRegistration &reg : m_registrations) {
            if (reg.def.opt_key != opt_key)
                continue;
            print_steps.insert(print_steps.end(), reg.invalidate_print_steps.begin(), reg.invalidate_print_steps.end());
            object_steps.insert(object_steps.end(), reg.invalidate_object_steps.begin(), reg.invalidate_object_steps.end());
        }
    }
}

} // namespace Slic3r
