#include "PluginManager.hpp"

#include "PluginABI.hpp"
#include "WallGeneratorRegistry.hpp"
#include "InfillProviderRegistryImpl.hpp"
#include "WipeTowerShapeRegistry.hpp"

#include "libslic3r/Semver.hpp"
#include "libslic3r/miniz_extension.hpp"

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

#include <miniz.h>

namespace Slic3r {
namespace fs = boost::filesystem;

PluginManager &PluginManager::instance()
{
    static PluginManager manager;
    return manager;
}

void PluginManager::initialize()
{
    if (m_initialized)
        return;
    register_builtin_wipe_tower_shapes();
    register_wall_generator_defaults();
    register_infill_provider_defaults();
    m_initialized = true;
}

void PluginManager::shutdown()
{
    for (LoadedPlugin &plugin : m_plugins)
        plugin.library.unload();
    m_plugins.clear();
}

void PluginManager::apply_config_schema()
{
    m_global_config_schema.apply_to_print_config_def();
    m_global_config_schema.register_preset_options();

    for (const PluginEnumExtension &ext : m_global_config_schema.enum_extensions()) {
        auto &option_map = m_plugin_enum_values[ext.option_key];
        for (const std::string &enum_key : ext.enum_values) {
            if (option_map.find(enum_key) == option_map.end())
                option_map[enum_key] = m_next_plugin_enum_value++;
        }
    }
}

std::string PluginManager::platform_library_name(const std::string &base_name) const
{
#ifdef _WIN32
    if (base_name.size() > 4 && base_name.substr(base_name.size() - 4) == ".dll")
        return base_name;
    return base_name + ".dll";
#elif defined(__APPLE__)
    if (base_name.size() > 6 && base_name.substr(0, 3) == "lib")
        return base_name;
    return "lib" + base_name + ".dylib";
#else
    if (base_name.size() > 3 && base_name.substr(0, 3) == "lib")
        return base_name;
    return "lib" + base_name + ".so";
#endif
}

bool PluginManager::check_version_gate(const PluginManifest &manifest, std::string &error) const
{
    if (manifest.abi_version != 0 && manifest.abi_version != ORCA_PLUGIN_ABI_VERSION) {
        error = "plugin ABI version mismatch for " + manifest.id;
        return false;
    }
    if (!manifest.min_app_version.empty()) {
        if (auto min_version = Semver::parse(manifest.min_app_version)) {
            if (auto host_version = Semver::parse(SLIC3R_VERSION)) {
                if (*host_version < *min_version) {
                    error = "plugin requires newer host version: " + manifest.id;
                    return false;
                }
            }
        }
    }
    if (!manifest.max_app_version.empty()) {
        if (auto max_version = Semver::parse(manifest.max_app_version)) {
            if (auto host_version = Semver::parse(SLIC3R_VERSION)) {
                if (*host_version > *max_version) {
                    error = "plugin not compatible with host version: " + manifest.id;
                    return false;
                }
            }
        }
    }
    return true;
}

bool PluginManager::register_loaded_plugin(LoadedPlugin &plugin, std::string &error)
{
    if (plugin.registered)
        return true;

    auto main_fn = reinterpret_cast<OrcaPluginMainFn>(plugin.library.symbol("orca_plugin_main"));
    if (main_fn == nullptr) {
        error = "orca_plugin_main not found in plugin " + plugin.manifest.id;
        return false;
    }

    OrcaPluginDescriptor *descriptor = main_fn();
    if (descriptor == nullptr) {
        error = "orca_plugin_main returned null for plugin " + plugin.manifest.id;
        return false;
    }
    if (descriptor->abi_version != ORCA_PLUGIN_ABI_VERSION) {
        error = "plugin ABI handshake failed for " + plugin.manifest.id;
        return false;
    }
    if (descriptor->register_plugin == nullptr) {
        error = "plugin register callback missing for " + plugin.manifest.id;
        return false;
    }

    PluginContext ctx(m_global_config_schema,
                      m_wipe_tower_shapes,
                      m_wall_generators,
                      m_infill_providers,
                      m_slicing_hooks,
                      m_gui_support);
    ctx.set_plugin_id(plugin.manifest.id);
    descriptor->register_plugin(ctx);

    plugin.registered = true;
    BOOST_LOG_TRIVIAL(info) << "Registered Orca plugin: " << descriptor->name << " (" << descriptor->version << ")";
    return true;
}

bool PluginManager::load_unpacked_plugin(const std::string &plugin_root, std::string &error)
{
    const fs::path root(plugin_root);
    const fs::path manifest_path = root / "manifest.json";
    if (!fs::exists(manifest_path)) {
        error = "manifest.json not found in " + plugin_root;
        return false;
    }

    LoadedPlugin plugin;
    plugin.root_path = plugin_root;
    if (!PluginManifest::load_from_file(manifest_path.string(), plugin.manifest, error))
        return false;
    if (!check_version_gate(plugin.manifest, error))
        return false;

    fs::path lib_path = root / platform_library_name(plugin.manifest.entry_library);
    if (!fs::exists(lib_path)) {
        lib_path = root / plugin.manifest.entry_library;
    }
    if (!fs::exists(lib_path)) {
        fs::path platform_lib = root / "lib" / platform_library_name(plugin.manifest.entry_library);
        if (fs::exists(platform_lib))
            lib_path = platform_lib;
    }
    if (!fs::exists(lib_path)) {
        error = "entry library not found for plugin " + plugin.manifest.id;
        return false;
    }

    if (!plugin.library.load(lib_path.string(), error))
        return false;

    if (!register_loaded_plugin(plugin, error)) {
        plugin.library.unload();
        return false;
    }

    m_plugins.emplace_back(std::move(plugin));
    return true;
}

bool PluginManager::load_plugin_path(const std::string &path, std::string &error)
{
    initialize();
    fs::path p(path);
    if (fs::is_directory(p))
        return load_unpacked_plugin(p.string(), error);

    if (p.extension() == ".orcaplugin") {
        load_orcaplugin_package(path, error);
        return error.empty();
    }

    error = "plugin path is not a directory or .orcaplugin package: " + path;
    return false;
}

bool PluginManager::load_plugins_from_directory(const std::string &directory)
{
    initialize();
    if (!fs::exists(directory))
        return true;

    bool ok = true;
    for (fs::directory_iterator it(directory), end; it != end; ++it) {
        if (!fs::is_directory(it->status()))
            continue;
        std::string error;
        if (!load_unpacked_plugin(it->path().string(), error)) {
            BOOST_LOG_TRIVIAL(warning) << "Failed to load plugin from " << it->path().string() << ": " << error;
            ok = false;
        }
    }
    apply_config_schema();
    return ok;
}

void PluginManager::load_orcaplugin_package(const std::string &package_path, std::string &error)
{
    initialize();
    fs::path temp_dir = fs::temp_directory_path() / fs::unique_path("orcaplugin-%%%%_%%%%");
    fs::create_directories(temp_dir);

    // Reuse miniz extraction pattern from GUI install_plugin — extract zip to temp and load.
    mz_zip_archive zip{};
    mz_zip_zero_struct(&zip);
    if (!mz_zip_reader_init_file(&zip, package_path.c_str(), 0)) {
        error = "failed to open .orcaplugin archive: " + package_path;
        return;
    }

    if (!mz_zip_reader_extract_all_to_dir(&zip, temp_dir.string().c_str(), 0)) {
        mz_zip_reader_end(&zip);
        error = "failed to extract .orcaplugin archive: " + package_path;
        fs::remove_all(temp_dir);
        return;
    }
    mz_zip_reader_end(&zip);

    if (!load_unpacked_plugin(temp_dir.string(), error))
        fs::remove_all(temp_dir);
    else
        apply_config_schema();
}

bool PluginManager::deserialize_extended_enum(ConfigOption *opt, const ConfigOptionDef *optdef, const std::string &value) const
{
    if (optdef == nullptr || opt->type() != coEnum)
        return false;
    auto option_it = m_plugin_enum_values.find(optdef->opt_key);
    if (option_it == m_plugin_enum_values.end())
        return false;
    auto value_it = option_it->second.find(value);
    if (value_it == option_it->second.end())
        return false;
    opt->setInt(value_it->second);
    return true;
}

std::string PluginManager::enum_key_for_value(const std::string &option_key, int int_value) const
{
    auto option_it = m_plugin_enum_values.find(option_key);
    if (option_it == option_it->end())
        return {};
    for (const auto &kv : option_it->second) {
        if (kv.second == int_value)
            return kv.first;
    }
    return {};
}

int PluginManager::enum_value_for_key(const std::string &option_key, const std::string &enum_key) const
{
    auto option_it = m_plugin_enum_values.find(option_key);
    if (option_it == m_plugin_enum_values.end())
        return -1;
    auto value_it = option_it->second.find(enum_key);
    return value_it == option_it->second.end() ? -1 : value_it->second;
}

} // namespace Slic3r
