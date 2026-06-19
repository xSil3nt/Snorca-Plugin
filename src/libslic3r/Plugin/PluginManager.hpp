#ifndef slic3r_PluginManager_hpp_
#define slic3r_PluginManager_hpp_

#include "ConfigSchemaRegistry.hpp"
#include "InfillProviderRegistry.hpp"
#include "PluginGUISupport.hpp"
#include "PluginManifest.hpp"
#include "SharedLibrary.hpp"
#include "SlicingHookBus.hpp"
#include "WallGeneratorRegistry.hpp"
#include "WipeTowerShapeRegistry.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Slic3r {

struct LoadedPlugin
{
    PluginManifest manifest;
    SharedLibrary  library;
    std::string    root_path;
    bool           registered{false};
};

class PluginManager
{
public:
    static PluginManager &instance();

    void initialize();
    bool load_plugin_path(const std::string &path, std::string &error);
    bool load_plugins_from_directory(const std::string &directory);
    void load_orcaplugin_package(const std::string &package_path, std::string &error);
    void apply_config_schema();
    void shutdown();

    const std::vector<LoadedPlugin> &loaded_plugins() const { return m_plugins; }
    ConfigSchemaRegistry            &config_schema() { return m_global_config_schema; }
    WipeTowerShapeRegistry          &wipe_tower_shapes() { return m_wipe_tower_shapes; }
    WallGeneratorRegistry           &wall_generators() { return m_wall_generators; }
    InfillProviderRegistry          &infill_providers() { return m_infill_providers; }
    SlicingHookBus                  &slicing_hooks() { return m_slicing_hooks; }
    PluginGUISupport                &gui_support() { return m_gui_support; }

    bool deserialize_extended_enum(ConfigOption *opt, const ConfigOptionDef *optdef, const std::string &value) const;
    std::string enum_key_for_value(const std::string &option_key, int int_value) const;
    int         enum_value_for_key(const std::string &option_key, const std::string &enum_key) const;

private:
    PluginManager() = default;

    bool load_unpacked_plugin(const std::string &plugin_root, std::string &error);
    bool register_loaded_plugin(LoadedPlugin &plugin, std::string &error);
    bool check_version_gate(const PluginManifest &manifest, std::string &error) const;
    std::string platform_library_name(const std::string &base_name) const;

    bool                        m_initialized{false};
    std::vector<LoadedPlugin>   m_plugins;
    ConfigSchemaRegistry        m_global_config_schema;
    WipeTowerShapeRegistry      m_wipe_tower_shapes;
    WallGeneratorRegistry       m_wall_generators;
    InfillProviderRegistry      m_infill_providers;
    SlicingHookBus              m_slicing_hooks;
    PluginGUISupport            m_gui_support;
    std::map<std::string, std::map<std::string, int>> m_plugin_enum_values;
    int                         m_next_plugin_enum_value{100};
};

} // namespace Slic3r

#endif // slic3r_PluginManager_hpp_
