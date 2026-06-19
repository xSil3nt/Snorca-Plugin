#include "ConfigSchemaRegistry.hpp"



#include "libslic3r/Config.hpp"

#include "libslic3r/Preset.hpp"

#include "libslic3r/Print.hpp"

#include "libslic3r/PrintConfig.hpp"



#include <boost/log/trivial.hpp>



namespace Slic3r {



namespace {



ConfigOptionDef plugin_def_to_host_def(const PluginConfigOptionDef &plugin_def)

{

    ConfigOptionDef def;

    def.opt_key = plugin_def.opt_key;

    def.type    = ConfigOptionType(plugin_def.type);

    if (!plugin_def.label.empty())

        def.label = plugin_def.label;

    if (!plugin_def.tooltip.empty())

        def.tooltip = plugin_def.tooltip;

    if (!plugin_def.sidetext.empty())

        def.sidetext = plugin_def.sidetext;

    if (!plugin_def.enum_values.empty())

        def.enum_values = plugin_def.enum_values;

    if (!plugin_def.enum_labels.empty())

        def.enum_labels = plugin_def.enum_labels;

    if (!plugin_def.default_string.empty()) {
        if (def.type == coFloat || def.type == coPercent)
            def.set_default_value(new ConfigOptionFloat(float(atof(plugin_def.default_string.c_str()))));
        else if (def.type == coInt)
            def.set_default_value(new ConfigOptionInt(atoi(plugin_def.default_string.c_str())));
        else if (def.type == coString)
            def.set_default_value(new ConfigOptionString(plugin_def.default_string));
    } else if (def.type == coFloat || def.type == coPercent) {
        def.set_default_value(new ConfigOptionFloat(plugin_def.default_float));
    } else if (def.type == coInt) {
        def.set_default_value(new ConfigOptionInt(plugin_def.default_int));
    }

    if (plugin_def.min_float != plugin_def.max_float || plugin_def.min_float != 0.f) {

        def.min = plugin_def.min_float;

        def.max = plugin_def.max_float;

    }

    return def;

}



} // namespace



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

        ConfigOptionDef *optdef = const_cast<ConfigOptionDef *>(def.get(ext.option_key));

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

        ConfigOptionDef host_def = plugin_def_to_host_def(reg.def);

        if (def.has(host_def.opt_key)) {

            BOOST_LOG_TRIVIAL(warning) << "ConfigSchemaRegistry: option already exists: " << host_def.opt_key;

            continue;

        }

        def.options.emplace(host_def.opt_key, host_def);

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



void ConfigSchemaRegistry::merge_invalidation(const std::vector<std::string> &opt_keys,

                                              std::vector<int> &print_steps,

                                              std::vector<int> &object_steps) const

{

    std::lock_guard<std::mutex> lock(m_mutex);

    for (const std::string &opt_key : opt_keys) {

        for (const PluginConfigRegistration &reg : m_registrations) {

            if (reg.def.opt_key != opt_key)

                continue;

            print_steps.insert(print_steps.end(), reg.invalidate_print_steps.begin(), reg.invalidate_print_steps.end());

            object_steps.insert(object_steps.end(), reg.invalidate_object_steps.begin(), reg.invalidate_object_steps.end());

        }

    }

}



} // namespace Slic3r

